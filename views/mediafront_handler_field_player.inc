<?php

/**
 * Field handler to provide simple renderer that turns a URL into a clickable link.
 *
 * @ingroup views_field_handlers
 */
class mediafront_handler_field_player extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();
    $options = array_merge( $options, array(
      'preset' => array('default' => ''),
      'intro' => array('default' => ''),
      'commercial' => array('default' => ''),
      'prereel' => array('default' => ''),
      'media' => array('default' => ''),
      'postreel' => array('default' => ''),
      'preview' => array('default' => '')
    ));
    return $options;
  }

  /**
   * Provide link to the page being visited.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $fields = array('' => t('<None>'));
    foreach ($this->view->display_handler->get_handlers('field') as $field => $handler) {
      if( strpos($field, 'mediafront_player') === FALSE ) {
        if ($label = $handler->label()) {
          $fields[$field] = $label;
        }
        else {
          $fields[$field] = $handler->ui_name();
        }
      }
    }

    $form['mediafront'] = array(
      '#type' => 'fieldset',
      '#title' => t('Media Player Settings')
    );

    $form['mediafront']['preset'] = mediafront_preset_select_form($this->options, 'preset');

    $form['mediafront']['mediawrapper'] = array(
      '#type' => 'fieldset',
      '#title' => t('Media Settings')
    );

    $form['mediafront']['mediawrapper']['intro'] = array(
      '#type' => 'select',
      '#title' => t('Intro Media Field'),
      '#options' => $fields,
      '#default_value' => $this->options['intro'],
      '#description' => t('The intro is the media that plays before all other media.'),
    );

    $form['mediafront']['mediawrapper']['commercial'] = array(
      '#type' => 'select',
      '#title' => t('Commercial Media Field'),
      '#options' => $fields,
      '#default_value' => $this->options['commercial'],
      '#description' => t('The commercial media will block out all media controls while it is playing.'),
    );

    $form['mediafront']['mediawrapper']['prereel'] = array(
      '#type' => 'select',
      '#title' => t('Pre-reel Media Field'),
      '#options' => $fields,
      '#default_value' => $this->options['prereel'],
      '#description' => t('The pre-reel plays before the main media.'),
    );

    $form['mediafront']['mediawrapper']['media'] = array(
      '#type' => 'select',
      '#title' => t('Main Media Field'),
      '#options' => $fields,
      '#default_value' => $this->options['media'],
      '#description' => t('The main media.'),
    );

    $form['mediafront']['mediawrapper']['postreel'] = array(
      '#type' => 'select',
      '#title' => t('Post-reel Media Field'),
      '#options' => $fields,
      '#default_value' => $this->options['postreel'],
      '#description' => t('The post-reel plays after the main media.'),
    );

    $form['mediafront']['previewwrapper'] = array(
      '#type' => 'fieldset',
      '#title' => t('Image Settings')
    );

    $form['mediafront']['previewwrapper']['preview'] = array(
      '#type' => 'select',
      '#title' => t('Preview Image Field'),
      '#options' => $fields,
      '#default_value' => $this->options['preview'],
      '#description' => t('The image field to show as the preview image.'),
    );
  }

  function  options_submit($form, &$form_state) {
    parent::options_submit($form, $form_state);
    // Flatten all mediafront options
    $form_state['values']['options']['preset'] = $form_state['values']['options']['mediafront']['preset'];
    $form_state['values']['options'] = array_merge($form_state['values']['options'], $form_state['values']['options']['mediafront']['mediawrapper']);
    $form_state['values']['options'] = array_merge($form_state['values']['options'], $form_state['values']['options']['mediafront']['previewwrapper']);
  }

  // Override the query so that the field is not added.
  function query() {}
  
  // Direct copy and paste from content_handler_field render routine.
  function renderField( $values, $fieldObj ) {
    // We're down to a single node here, so we can retrieve the actual field
    // definition for the node type being considered.
    $field = content_fields($fieldObj->content_field['field_name'], $values->{$fieldObj->aliases['type']});

    // If the field does not appear in the node type, then we have no value
    // to display, and can just return.
    if (empty($field)) {
      return '';
    }

    $options = $fieldObj->options;
    $db_info = content_database_info($field);

    // Build a pseudo-node from the retrieved values.
    $node = drupal_clone($values);
    $node->type = $values->{$fieldObj->aliases['type']};
    $node->nid = $values->{$fieldObj->aliases['nid']};
    $node->vid = $values->{$fieldObj->aliases['vid']};
    $node->build_mode = NODE_BUILD_NORMAL;

    $item = array();
    foreach ($db_info['columns'] as $column => $attributes) {
      $item[$column] = $values->{$fieldObj->aliases[$attributes['column']]};
    }

    $item['#delta'] = $field['multiple'] ?  $values->{$fieldObj->aliases['delta']} : 0;

    // Render items.
    $formatter_name = $options['format'];
    if ($formatter = _content_get_formatter($formatter_name, $field['type'])) {
      if (content_handle('formatter', 'multiple values', $formatter) == CONTENT_HANDLE_CORE) {
        // Single-value formatter.
        $output = content_format($field, $item, $formatter_name, $node);
      }
      else {
        // Multiple values formatter - we actually have only one value to display.
        $output = content_format($field, array($item), $formatter_name, $node);
      }
      return $fieldObj->render_link($output, $values);
    }
    return '';
  }
  
  function render($values) {    
    $options = array(
      'intro' => $this->options['intro'],
      'commercial' => $this->options['commercial'],
      'prereel' => $this->options['prereel'],
      'media' => $this->options['media'],
      'postreel' => $this->options['postreel'],
      'preview' => $this->options['preview']
    );
    
    $node = new stdClass();
    $node->mediafiles = array(
      'media' => array(),
      'images' => array('image' => '')
    );    
    
    foreach( array_filter($options) as $type => $value ) {
      if( isset($this->view->field[$value]) )  {
        $field = $this->view->field[$value];
        $value = $this->renderField($values, $field);
        if( $type == 'preview' ) {
          $node->mediafiles['images']['image'] = $value;
        }
        else {
          $node->mediafiles['media'][$type] = $value;
        }
      }
    }

    return theme('mediafront_field', $node, $this->options['preset']);
  }
}
