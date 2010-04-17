<?php
/**
 * @file
 * Provides CCK integration for the mediafront module
 */

/**
 * Implementation of hook_field_info().
 */
function mediafront_field_info() {
  return array(
    'mediafront' => array(
      'label' => t('Media Player'),
      'description' => t('Add a media player to this content.'),
    ),
  );
}

/**
 * Implementation of hook_field_settings().
 */
function mediafront_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      // Multiple is not supported with MediaFront.
      $form['multiple'] = array(
        '#type' => 'value',
        '#value' => 0,
      );

      if (user_access('use PHP for mediafront parameters')) {
        $form['params'] = array(
          '#type' => 'fieldset',
          '#title' => t('Additional Parameters'),
          '#collapsible' => TRUE,
          '#collapsed' => empty($field['php_params']),
        );

        $form['params']['params'] = array(
          '#title' => t('Code'),
          '#type' => 'textarea',
          '#default_value' => $field['php_params'] ? $field['php_target'] : '',
          '#description' => t('Advanced usage only: PHP code that returns an array of additional parameters to override the preset. Should not include &lt;?php ?&gt; delimiters. If this field is filled out, the value returned by this code will override any value specified above. Note that executing incorrect PHP-code can break your Drupal site.'),
        );
      }
      else {
        $form['params'] = array(
          '#type' => 'value',
          '#value' => $field['params'] ? $field['params'] : '',
        );
      }

      return $form;
    case 'save':
      return array('params');
    case 'database columns':
      return array('params' => array('type' => 'text', 'size' => 'medium', 'default' => '', 'sortable' => FALSE));
  }
}

/**
 * Implementation of hook_field() 
 */
function mediafront_field($op, &$node, $field, &$items, $teaser, $page) {
   if($op == 'view') {
      foreach ($items as $delta => $item) {
        $items[$delta]['view'] = content_format($field, $item, 'default', $node);
      }
      return theme('field', $node, $field, $items, $teaser, $page);
  }
}

/**
 * Implementation of hook_field_formatter_info().
 */
function mediafront_field_formatter_info() {
  $formatters = array();
  foreach (mediafront_preset_get_presets() as $preset) {
    $formatters[$preset->name . '_player'] = array(
      'label' => t('@preset', array('@preset' => $preset->name)),
      'field types' => array('mediafront'),
    );
  }
  return $formatters;
}

/**
 * Implementation of hook_widget_info().
 */
function mediafront_widget_info() {
  return array(
    'player' => array(
      'label' => t('Media Player'),
      'field types' => array('mediafront'),
      'multiple values' => CONTENT_HANDLE_MODULE,
    ),
  );
}

/**
 * Implementation of hook_content_is_empty().
 */
function mediafront_content_is_empty($item, $field) {
  return FALSE;
}

/**
 * Implementation of hook_widget_settings().
 */
function mediafront_widget_settings($op, $widget) {
}

/**
 * Implementation of hook_widget().
 */
function mediafront_widget(&$form, &$form_state, $field, $items) {
  $element = array('#tree' => TRUE);

  $element['params'] = array(
    '#type' => 'value',
    '#value' => $field['params'],
  );

  // CCK likes to always have a 2D array for form elements.
  $element = array($element);

  return $element;
}
