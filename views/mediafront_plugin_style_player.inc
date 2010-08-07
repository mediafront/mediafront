<?php
// $Id$

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
class mediafront_plugin_style_player extends views_plugin_style {
   
  /**
   * Set the default options.
   */   
  function option_definition() {
    $options = parent::option_definition();
    $options['mediafront_preset'] = array('default' => '');
    return $options;
  }

  /**
   * Shows the options form for the media player style
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $presets = mediafront_preset_get_presets();
    $options = array();
    foreach( $presets as $preset ) {
      $options[$preset['name']] = $preset['name'];
    }
    $form['mediafront_preset'] = array(
      '#type' => 'select',
      '#title' => t('MediaFront Presets'),
      '#options' => $options,
      '#default_value' => isset($this->options['mediafront_preset']) ? $this->options['mediafront_preset'] : '',
    );
  }

  /**
   * Override the query to tell our view to get the total number of rows.
   */
  function query() {
    parent::query();
    $this->view->get_total_rows = TRUE;
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

    // Now render the media player for this view.
    return theme($this->theme_functions(), $this->view, $this->options['mediafront_preset']);
  }  
}
