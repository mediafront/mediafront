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
  $views_path = drupal_get_path('module', 'views');
  $plugins = array(
    'module' => 'mediafront', // This just tells our themes are elsewhere.
    'display' => array(
      // Parents are not really displays, just needed so the files can
      // be included.
      'parent' => array(
        'no ui' => TRUE,
        'handler' => 'views_plugin_display',
        'path' => "$views_path/plugins",
        'parent' => '',
      ),
      'block' => array(
        'no ui' => TRUE,
        'handler' => 'views_plugin_display_block',
        'path' => "$views_path/plugins",
        'parent' => 'parent',
      ),   
      'page' => array(
        'no ui' => TRUE,
        'handler' => 'views_plugin_display_page',
        'path' => "$views_path/plugins",
        'parent' => 'parent',
      ),           
      // Display plugin for Media Player Page.
      'mediaplayer' => array(
        'title' => t('Media Player Block'),
        'help' => t('Shows all of the items in this view within a Media Player Block.'),
        'handler' => 'mediafront_plugin_display_block',
        'parent' => 'block',        
        'path' => "$path/includes",
        'theme' => 'views_view',
        'uses hook block' => TRUE,
        'use ajax' => TRUE,
        'accept attachments' => TRUE,
      ),
      // Display plugin for Media Player Page.
      'mediaplayerpage' => array(
        'title' => t('Media Player Page'),
        'help' => t('Shows all of the items in this view within a Media Player on a Page.'),
        'handler' => 'mediafront_plugin_display_page',
        'parent' => 'page',        
        'path' => "$path/includes",
        'uses hook menu' => TRUE,        
        'theme' => 'views_view',
        'use ajax' => TRUE,
        'accept attachments' => TRUE,
      ),      
    ), 
  );
  return $plugins;
}

/**
 * Implementation of hook_views_data()
 */
function mediafront_views_data() {
  /******************************************************************************
   * I know... I know... this is a hack.  But, as far as I can tell, is necessary.
   * I need to allow the user to configure which preset they want to see for
   * any display within that view.  The best way I can figure to do this is to 
   * simply make the preset a field which they add to the view, where they can then 
   * add the preset as an option.
   *
   * The side effect for this design is that the mediafront_preset table is linked
   * to the node using the preset Id.  There is no relation between those two ( ugh ).
   * Hopefully this won't cause any issues, but I am certainly up for any suggestions 
   * that anyone has.
   ******************************************************************************/

  // Add the media front group.
  $data['mediafront_preset']['table']['group']  = t('MediaFront');
  $data['mediafront_preset']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'pid',
    )
  );
  
  $data['mediafront_preset']['name'] = array(
    'title' => t('Preset'),
    'help' => t('Select the preset that you would like to use for this view.'),
    'field' => array(
      'handler' => 'mediafront_handler_field_preset',
      'click sortable' => FALSE,
    ),
  );

  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function mediafront_views_handlers() {
  return array(
    'handlers' => array(
      'mediafront_handler_field_preset' => array(
        'parent' => 'views_handler_field',
        'path' => drupal_get_path('module', 'mediafront') . '/includes',
      )
    )
  );
}
