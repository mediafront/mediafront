<?php
// $Id: 
/**
 * @file mediafront.views.inc
 * Built in plugins for Views output handling.
 *
 */

/**
 * Implementation of hook_views_plugins
 */
function mediafront_views_plugins() {
  $path = drupal_get_path('module', 'mediafront');
  $plugins = array(
    'module' => 'mediafront', // This just tells our themes are elsewhere.
    'style' => array(
      'mediaplayer' => array(
        'title' => t('Media Player'),
        'help' => t('Shows this view as a Media Player.'),
        'handler' => 'mediafront_plugin_style_player',
        'theme' => 'mediafront_player_view',
        'path' => "$path/views",
        'theme path' => "$path/views",
        'uses fields' => FALSE,
        'uses row plugin' => FALSE,
        'uses options' => TRUE,
        'uses grouping' => FALSE,
        'type' => 'normal',
        'even empty' => TRUE,
      )
    )    
  );
  return $plugins;
}

/**
 * Implementation of hook_views_handlers
 */
function mediafront_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'mediafront') . '/views',
    ),
    'handlers' => array(
      'mediafront_handler_field_player' => array(
        'parent' => 'views_handler_field',
      ),
    )
  );
}

/**
 * Implementation of hook_views_data()
 */
function mediafront_views_data() {
  $data['views']['mediafront_player'] = array(
    'title' => t('Media Player'),
    'help' => t('Add a media player field to this view.'),
    'field' => array(
      'handler' => 'mediafront_handler_field_player',
      'click sortable' => FALSE,
    ),
  );
  return $data;
}
