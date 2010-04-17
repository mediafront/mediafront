<?php
// $Id$

/**
 * @file
 * This include file implements views functionality on behalf of the
 * flashvideo.module.
 */

/**
 * A handler to provide proper displays for videos.
 */
class mediafront_handler_field_preset extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();
    $options['mediafront_preset'] = array('default' => '');
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $presets = mediafront_preset_get_presets();
    $options = array();
    foreach( $presets as $preset ) {
      $options[$preset->name] = $preset->name;
    }
    $form['mediafront_preset'] = array(
      '#type' => 'select',
      '#title' => t('MediaFront Presets'),
      '#options' => $options,
      '#default_value' => isset($this->options['mediafront_preset']) ? $this->options['mediafront_preset'] : '',
    );
  }
}