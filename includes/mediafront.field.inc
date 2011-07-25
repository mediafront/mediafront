<?php
/**
 * Implements hook_field_info().
 */
function mediafront_field_info() {
  $presets = mediafront_preset_get_presets();
  $preset = array_shift( $presets );
  $default_formatter = ($preset) ? 'mediaplayer__' . $preset['name'] : 'hidden';

  return array(
    'mediaplayer' => array(
      'label' => t('Media Player'),
      'description' => t('Add a media player to this entity.'),
      'default_widget' => 'mediaplayer',
      'default_formatter' => $default_formatter,
      'instance_settings' => array(
        'show_player' => true
      ),
    ),
  );
}

/**
 * Implements hook_field_schema().
 */
function mediafront_field_schema($field) {
  return array(
    'columns' => array(
      'fid' => array(
        'description' => 'The file field being referenced.',
        'type' => 'int',
        'not null' => FALSE,
        'unsigned' => TRUE,
      ),
      'name' => array(
        'description' => "The name of the file field.",
        'type' => 'varchar',
        'length' => 128,
        'not null' => FALSE,
      ),
    ),
    'indexes' => array(
      'fid' => array('fid'),
    ),
  );
}

/**
 * Implements hook_field_widget_info().
 */
function mediafront_field_widget_info() {
  return array(
    'mediaplayer' => array(
      'label' => t('Media Player'),
      'description' => t('Add a media player to this entity.'),
      'field types' => array('mediaplayer')
    ),
  );
}

/**
 * Implements hook_field_formatter_info().
 */
function mediafront_field_formatter_info() {
  $formatters = array();

  foreach (mediafront_preset_get_presets() as $preset) {
    $formatters['mediaplayer__' . $preset['name']] = array(
      'label' => t('@preset', array('@preset' => $preset['name'])),
      'description' => t('Allows the Media Player field to be displayed.'),
      'field types' => array('mediaplayer'),
    );
  }
  return $formatters;
}

/**
 * Implements hook_field_formatter_view().
 */
function mediafront_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  // Check if the formatter involves a particular image style.
  $matches = array();
  if (preg_match('/__([a-z0-9_]+)/', $display['type'], $matches)) {
    $preset = $matches[1];
  }

  $element[] = array(
    '#theme' => 'mediafront_player_formatter',
    '#entity' => $entity,
    '#preset' => isset($preset) ? $preset : ''
  );

  return $element;
}

/**
 * Implements hook_field_is_empty
 */
function mediafront_field_is_empty($item, $field) {
  return FALSE;
}

/**
 * Implements hook_field_prepare_view()
 */
function mediafront_field_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items)
{
  // Add a bogus item to the media players.
  foreach ($entities as $id => $entity) {
    $items[$id][0] = array('name' => 'mediaplayer');
  }
}

/**
 * Theme for the Media Player field.
 */
function theme_mediafront_player_formatter( $variables ) {
  $variables = array_shift( $variables );
  $preset = $variables['#preset'];
  $entity = $variables['#entity'];
  return theme('mediafront_player', array('entity' => $entity, 'preset' => $preset) );
}
?>