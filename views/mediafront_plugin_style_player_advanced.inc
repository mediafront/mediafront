<?php

/**
 * @file
 * This file holds style plugin for MediaFront Views
 *
 * @ingroup openlayers
 */

/**
 * @class
 * Extension of the Views Plugin Syle for the Media Player from MediaFront
 *
 * This class extended the default views plugin class to provide
 * a style plugin for the MediaFront module.
 */
class mediafront_plugin_style_player_advanced extends views_plugin_style {

  /**
   * Set the default options.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['mediafront_preset'] = array('default' => '');
    $options['mediafront_media_field'] = array('default' => '');
    $options['mediafront_image_field'] = array('default' => '');
    $options['mediafront_title_field'] = array('default' => '');
    return $options;
  }

  /**
   * Shows the options form for the media player style
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['mediafront_preset'] = mediafront_preset_select_form($this->options);
    $field_options = array('' => t('- None -'));
    $field_options += $this->display->handler->get_field_labels();
    $form['mediafront_media_field'] = array(
      '#type' => 'select',
      '#title' => t('Media (video/audio) field'),
      '#description' => t('Can be a text field outputting a URL, a file field or a media field.'),
      '#options' => $field_options,
      '#default_value' => $this->options['mediafront_media_field'],
      '#required' => TRUE,
    );
    $form['mediafront_image_field'] = array(
      '#type' => 'select',
      '#title' => t('Image field'),
      '#description' => t('Can be a text field outputting a URL, an image field or a media field.'),
      '#options' => $field_options,
      '#default_value' => $this->options['mediafront_image_field'],
    );
    $form['mediafront_title_field'] = array(
      '#type' => 'select',
      '#title' => t('Title field'),
      '#description' => t('This text field will be used as the video title.'),
      '#options' => $field_options,
      '#default_value' => $this->options['mediafront_title_field'],
    );
  }

  /**
   * Renders the media player.
   */
  function render() {
    $output = '';

    // Check for live preview.
    if (!empty($this->view->live_preview)) {
      return t('MediaFront views are not compatible with live preview.');
    }

    $i = 0;
    $params = array();
    $params['playlistObj']['nodes'] = array();
    foreach ($this->view->result as $index => $row) {
      $media_url = $this->getFieldRawUrl($index, $this->options['mediafront_media_field']);
      $this->view->row_index = $index;
      if ($media_url) {
        $playlist_node = array(
          'mediafiles' => array(
            'media' => array(
              'media' => (object)array(
                'path' => $media_url,
              ),
            ),
          ),
        );
        if ($this->options['mediafront_title_field']) {
          $title = $this->view->render_field($this->options['mediafront_title_field'], $index);
          if ($title) {
            $playlist_node['title'] = $title;
          }
        }
        if ($this->options['mediafront_image_field']) {
          $image_url = $this->getFieldRawUrl($index, $this->options['mediafront_image_field']);
          if ($image_url) {
            $playlist_node['mediafiles']['images'] = array(
              'image' => (object)array(
                'path' => $image_url,
              ),
            );
          }
        }
        $params['playlistObj']['nodes'][] = $playlist_node;
      }
    }

    $params['playlistObj']['total_rows'] = count($params['playlistObj']['nodes']);

    // Now render the media player for this view.
    return mediafront_get_player($this->options['mediafront_preset'], $params);
  }

  /**
   * Returns a full URL to a media or file field.
   * Accepts media fields and raw text fields.
   */
  function getFieldRawUrl($row_id, $field_name) {
    $media_url = '';
    if (isset($this->display->handler->handlers['field'][$field_name]->field_info['type'])) {
      $field_type = $this->display->handler->handlers['field'][$field_name]->field_info['type'];
      switch ($field_type) {
        case 'file':
        case 'image':
        case 'media':
          if (isset($this->view->result[$row_id]->{"field_$field_name"}[0])) {
            if ($field_type == 'media') {
              $uri = $this->view->result[$row_id]->{"field_$field_name"}[0]['raw']['file']->uri;
            }
            else {
              $uri = $this->view->result[$row_id]->{"field_$field_name"}[0]['raw']['uri'];
            }
            $stream_wrapper = file_stream_wrapper_get_instance_by_uri($uri);
            if (is_object($stream_wrapper)) {
              $media_url = $stream_wrapper->getExternalUrl();
            }
          }
          break;
        default:
          break;
      }
    }
    if (!$media_url) {
      $media_url = $this->view->render_field($field_name, $row_id);
    }
    return $media_url;
  }

}
