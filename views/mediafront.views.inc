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
