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
    $form['mediafront_preset'] = mediafront_preset_select_form($this->options);
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
    return theme($this->theme_functions(), array( 'view' => $this->view, 'preset' => $this->options['mediafront_preset'] ));
  }
}
