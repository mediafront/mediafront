<?php
define('MEDIAFRONT_PRESET_MAX_LENGTH', 64);

/**
 * Implements hook_perm().
 */
function mediafront_preset_perm() {
  return array('administer mediafront presets');
}

/**
 * Convenience function to get the path of the module presets.
 *
 * @return <string> The path to the module presets.
 */
function mediafront_get_presets_path() {
  return 'admin/build/mediafront';
}

/**
 * Implement the menu system for the preset.
 */
function mediafront_preset_menu() {
  $items = array();

  // Get the presets path.
  $presets_path = mediafront_get_presets_path();

  $items[$presets_path] = array(
    'title' => 'MediaFront Presets',
    'description' => 'Manage and configure the presets for MediaFront.',
    'page callback' => 'mediafront_presets_overview',
    'access arguments' => array('administer mediafront presets')
  );
  $items[$presets_path . '/list'] = array(
    'title' => 'List',
    'access arguments' => array('administer mediafront presets'),
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => -10,
  );
  $items[$presets_path . '/add'] = array(
    'title' => 'Add preset',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mediafront_preset_create_form'),
    'access arguments' => array('administer mediafront presets'),
    'type' => MENU_LOCAL_TASK
  );
  $items[$presets_path . '/import'] = array(
    'title' => t('Import preset'),
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mediafront_preset_import_form'),
    'access arguments' => array('administer mediafront presets'),
    'type' => MENU_LOCAL_TASK
  );

  $file = 'includes/mediafront.preset.inc';
  $items[$presets_path . '/preset/%mediafront_preset'] = array(
    'title' => 'Edit MediaFront Preset',
    'title callback' => 'mediafront_preset_page_title',
    'title arguments' => array(4),
    'file' => $file,
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mediafront_preset_form', 4),
    'access arguments' => array('administer mediafront presets')
  );
  $items[$presets_path . '/preset/%mediafront_preset/edit'] = array(
    'title' => 'Edit',
    'file' => $file,
    'page arguments' => array(4),
    'access arguments' => array('administer mediafront presets'),
    'type' => MENU_DEFAULT_LOCAL_TASK,
  );
  $items[$presets_path . '/preset/%mediafront_preset/delete'] = array(
    'title' => 'Delete',
    'file' => $file,
    'page arguments' => array('mediafront_preset_delete_confirm', 4),
    'access arguments' => array('administer mediafront presets'),
    'type' => MENU_CALLBACK
  );
  $items[$presets_path . '/preset/%mediafront_preset/export'] = array(
    'title' => t('Export'),
    'file' => $file,
    'title callback' => 'mediafront_preset_page_title',
    'title arguments' => array(4),
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mediafront_preset_export_form', 4),
    'access arguments' => array('administer mediafront presets'),
    'type' => MENU_CALLBACK
  ); 
   
  return $items;
}

/**
 * Implementation of hook_theme
 */
function mediafront_preset_theme() {
  $theme = array();

  // Iterate through all of our presets, and create a formatter for each one.
  foreach(mediafront_preset_get_presets() as $preset) {
    $theme['mediafront_formatter_'. $preset['name'] .'_player'] = array(
      'arguments' => array('element' => NULL),
      'function' => 'theme_mediafront_formatter_player',
    );
  }

  return $theme;
}

/**
 * Implement the theme for the CCK fields.
 */
function theme_mediafront_formatter_player($element) {
  // Extract the preset name from the formatter name.
  $preset = substr($element['#formatter'], 0, strrpos($element['#formatter'], '_'));

  // Return a node mediafront player.
  return theme('mediafront_player', $element['#node'], $preset );
}


/**
 * Menu callback: MediaFront Module administration.
 */
function mediafront_presets_overview() {
  $presets = mediafront_preset_get_presets();
  $presets_path = mediafront_get_presets_path();
  
  if( count( $presets ) > 0 ) {
    $header = array(t('Preset Name'), t('Description'), array('data' => t('Operations'), 'colspan' => '3'));
    $rows = array();
    foreach( $presets as $pid => $preset ) {
        $row = array();
        $row[] = check_plain($preset['name']);
        $row[] = array('data' => filter_xss_admin($preset['description']), 'class' => 'description');
        $row[] = array('data' => l(t('edit'), $presets_path . '/preset/'. $preset['name']));
        
        if( !isset($preset['default']) ) {
          $row[] = array('data' => l(t('delete'), $presets_path . '/preset/'. $preset['name'] .'/delete'));
        }
        
        $row[] = array('data' => l(t('export'), $presets_path . '/preset/'. $preset['name'] .'/export'));
        $rows[] = $row;
    }
    return theme('table', $header, $rows);
  }
  else {
    return 'No presets.  To add a new preset, click on the <strong>Add Preset</strong> button.';
  }
}

function mediafront_preset_form(&$form_state, $preset) {
   $form['preview'] = array(
      '#type' => 'fieldset',
      '#title' => t('Player Preview'),
      '#attributes' => array('id' => 'mediafront_admin_preview'),
      '#weight' => -10
   );

   // Set up the parameters for the player.
   $preset = (array)$preset;
   $params = array( 'admin' => true, 'player' => $preset['player'], 'preset' => $preset['name'] );
   $params = $preset['settings'] ? array_merge( $preset['settings'], $params ) : $params;

   $form['preview']['player'] = array(
      '#type' => 'markup',
      '#value' => mediafront_get_player($params)
   );

   // Get all of the settings for this player.
   $preset['settings'] = mediafront_get_settings( $preset['player'], $params );

   $form['settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Preset Settings'),
      '#attributes' => array('id' => 'mediafront_admin_settings'),     
      '#weight' => -9,
      '#collapsible' => false,
      '#collapsed' => false   
   );
   $form['settings']['preset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Preset Information'),    
      '#weight' => -9,
      '#collapsible' => true,
      '#collapsed' => true
   );

   if( isset($preset['pid']) ) {
     $form['settings']['preset']['pid'] = array(
        '#type' => 'value',
        '#value' => $preset['pid'],
     );
   }
   
   $form['settings']['preset']['name'] = array(
      '#type' => 'value',
      '#value' => $preset['name'],
   );   
   $form['settings']['preset']['description'] = array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#description' => t('Add a brief description to your preset name.'),
      '#default_value' => $preset['description'],
      '#weight' => -9,
   );
   $form['settings']['preset']['player'] = array(
      '#type' => 'value',
      '#value' => $preset['player'],
   );    
   
   $form['settings']['player_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Player Settings'),      
      '#weight' => -8,
      '#collapsible' => true,
      '#collapsed' => true,
      '#tree' => TRUE,
   );
   
   // Now add the player settings form to this fieldset.
   $form['settings']['player_settings'] = array_merge( mediafront_player_settings_form($preset), $form['settings']['player_settings'] );
   
   // Add the player to player settings.
   $form['settings']['connect'] = array(
      '#type' => 'fieldset',
      '#title' => t('Player to Player'),  
      '#collapsible' => true,
      '#collapsed' => true
   );
   
   $result = mediafront_preset_get_presets();
   $presets = array();
   foreach( $result as $pid => $otherPreset ) {
      if( $otherPreset['name'] != $preset['name'] ) {
         $presets[$otherPreset['name']] = $otherPreset['name'];
      }
   }

   if (isset($preset['connect']) && $preset['connect'] ) {
     $form['settings']['connect']['iscontroller'] = array(
        '#type' => 'select',
        '#multiple' => TRUE,
        '#title' => t('Make this preset the Control Bar for'),
        '#description' => t('Select which preset you would like to control using this preset.'),
        '#default_value' => $preset['connect']['iscontroller'],
        '#options' => $presets
     );

     $form['settings']['connect']['isplaylist'] = array(
        '#type' => 'select',
        '#multiple' => TRUE,
        '#title' => t('Make this preset the Playlist for'),
        '#description' => t('Select which preset you would like to play this playlist.'),
        '#default_value' => $preset['connect']['isplaylist'],
        '#options' => $presets
     );
   }
   
   $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save Preset'),
      '#weight' => 30,
   );   
   
   $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#weight' => 40,
      '#submit' => array('mediafront_preset_delete_submit'),
    );    
   
   // Add the form submit handler.
   $form['#submit'][] = 'mediafront_preset_submit';
   $form['#validate'][] = 'mediafront_preset_validate';   
   return $form;
}

/**
 * Validation for the preset form.
 */
function mediafront_preset_validate($form, &$form_state) {
}

/**
 * Flatten the settings array.
 *
 * @param <type> $array
 * @param <type> $return
 * @return <type>
 */
function mediafront_preset_array_flatten($array,$return=array()) {
  foreach( $array as $key => $value ) {
    if( is_array($value) ) {
      $return = mediafront_preset_array_flatten($value, $return);
    }
    else {
      $return[$key] = $value;
    }
  }
  return $return;
}

/**
 * Submit handler for the preset form.
 */
function mediafront_preset_submit($form, &$form_state) {
  $preset = array();
  if( isset($form_state['values']['pid']) ) {
    $preset['pid'] = $form_state['values']['pid'];
  }
  $preset['name'] = $form_state['values']['name'];
  $preset['description'] = $form_state['values']['description'];
  $preset['player'] = $form_state['values']['player'];
  $preset['connect'] = array(
     'iscontroller' => isset($form_state['values']['iscontroller']) ? $form_state['values']['iscontroller'] : array(),
     'isplaylist' => isset($form_state['values']['isplaylist']) ? $form_state['values']['isplaylist'] : array()
  );
  
  // Now set the player settings that are only different from the defaults.  
  $preset['settings'] = mediafront_get_settings( $preset['player'], mediafront_preset_array_flatten($form_state['values']['player_settings']) );

  // Save this preset.
  mediafront_preset_save( $preset );
  $form_state['redirect'] = mediafront_get_presets_path() . '/preset/' . $preset['name'];
}

/**
 * Button sumit function: handle the 'Delete' button on the preset form.
 */
function mediafront_preset_delete_submit($form, &$form_state) {
  $preset['name'] = $form_state['values']['name'];
  $form_state['redirect'] = mediafront_get_presets_path() . '/preset/' . $preset['name'] . '/delete';
}

/**
 * Menu callback; delete a single preset.
 */
function mediafront_preset_delete_confirm(&$form_state, $preset) {
  $form['name'] = array('#type' => 'value', '#value' => $preset['name']);
  $message = t('Are you sure you want to delete the preset %name?', array('%name' => $preset['name']));
  $caption = '<p>' . t('This action cannot be undone.') . '</p>';
  return confirm_form($form, $message, mediafront_get_presets_path(), $caption, t('Delete'));
}

/**
 * Execute preset deletion
 */
function mediafront_preset_delete_confirm_submit($form, &$form_state) {
  if( $form_state['values']['confirm'] ) {
    mediafront_preset_delete($form_state['values']['name']);
  }

  $form_state['redirect'] = mediafront_get_presets_path();
}

/**
 * Form builder; the create preset form.
 */
function mediafront_preset_create_form(&$form_state) {
  $form['preset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Preset Information'),
    '#weight' => -10,
  );

  $form['preset']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Preset Name'),
    '#maxlength' => MEDIAFRONT_PRESET_MAX_LENGTH,
    '#description' => t('Spaces are NOT allowed; punctuation is not allowed except for periods, hyphens, apostrophes, and underscores.'),
    '#required' => TRUE,
    '#weight' => -10,
  );

  $form['preset']['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('Add a brief description to your preset name.'),
    '#weight' => -9,
  );

  // Get all of the available players.
  $players = mediafront_get_players();
  $options = array();
  foreach( $players as $name => $player ) {
    $options[$name] = $player['title'];
  }

  $form['preset']['player'] = array(
    '#type' => 'select',
    '#title' => t('Media Player'),
    '#description' => t('Select which player you would like to use for this preset.'),
    '#options' => $options,
    '#weight' => -8,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
    '#weight' => 30,
  );

  // Add the form submit handler.
  $form['#submit'][] = 'mediafront_create_preset_submit';
  $form['#validate'][] = 'mediafront_create_preset_validate';

  return $form;
}

/**
 * Validation for the preset form.
 */
function mediafront_create_preset_validate($form, &$form_state) {
   if ($error = mediafront_validate_preset_name($form_state['values']['name'])) {
      form_set_error('name', $error);
   }
}

/**
 * Submit handler for the preset form.
 */
function mediafront_create_preset_submit($form, &$form_state) {
  $preset = $form_state['values'];
  $preset['name'] = strtolower( $preset['name'] );
  mediafront_preset_save($preset);
  $form_state['redirect'] = mediafront_get_presets_path() . '/preset/' . $preset['name'];
}

/**
 * Import's a preset.
 */
function mediafront_preset_import_form( &$form_state ) {
  $form['preset'] = array(
    '#title' => t('Preset Code'),
    '#type' => 'textarea',
    '#rows' => 10,
    '#description' => t('Copy the export text and paste it into this text field to import a new preset.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import Preset'),
    '#weight' => 30,
  );

  $form['#submit'][] = 'mediafront_preset_import_submit';
  $form['#validate'][] = 'mediafront_preset_import_validate';

  return $form;
}

/**
 * Validation for the preset import form.
 */
function mediafront_preset_import_validate($form, &$form_state) {
  $preset = '';

  // Get the preset that they declared in the text field.
  ob_start();
  eval($form_state['values']['preset']);
  ob_end_clean();

  if( is_array($preset) ) {
    $name = isset( $preset['name'] ) ? $preset['name'] : '';
    if ($error = mediafront_validate_preset_name($name)) {
      form_set_error('name', $error);
    }
  }
  else {
    form_set_error('name', 'Invalid preset import.');
  }

  $form_state['preset'] = &$preset;
}

/**
 * Submit handler for the preset import form.
 */
function mediafront_preset_import_submit($form, &$form_state) {
  $preset = $form_state['preset'];
  mediafront_preset_save( $preset );
  $form_state['redirect'] = mediafront_get_presets_path() . '/preset/' . $preset['name'];
}

/**
 * The preset export form.
 */
function mediafront_preset_export_form( &$form_state, $preset ) {
  // Unset the unnecessary elements.
  unset($preset['pid']);
  unset($preset['default']);

  // Get the code string representation.
  $code = var_export($preset, true);

  // Make sure to format the arrays like drupal.
  $code = str_replace("=> \n  ", '=> ', $code);

  // Add the preset variable.
  $code = '$preset = ' . $code . ';';

  $lines = substr_count($code, "\n");
  $form['export'] = array(
    '#title' => t('Export Preset'),
    '#type' => 'textarea',
    '#value' => $code,
    '#rows' => $lines,
    '#description' => t('Copy the export text and paste it into another preset using the import function.'),
  );

  // Return the form.
  return $form;
}

/**
 * Get's the player settings for the given player.
 */
function mediafront_player_settings_form( $preset ) {
  $settings_form = $preset['player'] . '_player_settings_form';
  if( function_exists( $settings_form ) ) {
    return $settings_form($preset);
  }
  return array();
}

/**
 * Title callback for a preset.
 */
function mediafront_preset_page_title($preset) {
  return $preset['name'];
}

/**
 * Saves a new preset.
 */
function mediafront_preset_save( $preset ) {
  // If they provide the pid, then this needs to be an update.
  $pid = (isset($preset['pid']) && $preset['pid']) ? array('pid') : array();

  // Save or update a preset.
  drupal_write_record( 'mediafront_preset', $preset, $pid );

  // Tell the player about this preset save.
  $player_save = $preset['player'] . '_mediafront_preset_save';
  if( function_exists( $player_save ) ) {
    $preset['settings'] = isset($preset['settings']) ? $preset['settings'] : array();
    $preset['settings'] = mediafront_get_settings( $preset['player'], $preset['settings'] );
    $player_save( $preset );
  }

  // Rebuild Theme Registry
  drupal_rebuild_theme_registry();
}

/**
 * Returns the default presets.
 * 
 * @return <type>
 */
function mediafront_preset_get_default_presets() {
  $default_presets = module_invoke_all('mediafront_default_presets');
  drupal_alter('mediafront_default_presets', $default_presets);
  return $default_presets;
}

/**
 * Gets a list of all presets.
 */
function mediafront_preset_get_presets() {
  $presets = array();
  $normal_presets = array();

  // Get all the presets from the database.
  $result = db_query("SELECT * FROM {mediafront_preset} ORDER BY name ASC");
  while( $preset = db_fetch_array($result) ) {
    $preset = (array)$preset;
    $preset['connect'] = $preset['connect'] ? unserialize( $preset['connect'] ) : array();
    $preset['settings'] = $preset['settings'] ? unserialize( $preset['settings'] ): array();
    $presets[$preset['pid']] = $preset;
    $normal_presets[$preset['name']] = $preset['pid'];
  }

  // Now allow other modules to add their default presets.
  foreach (mediafront_preset_get_default_presets() as $preset) {
    if (!empty($preset['name']) && !isset($normal_presets[$preset['name']])) {
      $presets[$preset['name']] = $preset;
    }
  }

  return $presets;
}

/**
 * Used for the menu item to load a preset.
 * 
 * @param <type> $preset_name 
 */
function mediafront_preset_load( $preset_name ) {
  return mediafront_get_preset( $preset_name );
}

/**
 * Get's the preset.
 */
function mediafront_get_preset( $preset_name ) {
  $preset = db_fetch_array( db_query("SELECT * FROM {mediafront_preset} WHERE name='%s'", $preset_name));
  if( $preset ) {
    $preset['settings'] = $preset['settings'] ? unserialize( $preset['settings'] ) : array();
    $preset['connect'] = $preset['connect'] ? unserialize( $preset['connect'] ) : array();
  }
  else {
    // Get all of the default presets.
    $default_presets = mediafront_preset_get_default_presets();

    // Check to see if this preset exists.
    if (isset($default_presets[$preset_name])) {
      // Make this our preset
      $preset = $default_presets[$preset_name];
    }
  }

  // Return the preset.
  return $preset;
}

/**
 * Deletes a preset
 */
function mediafront_preset_delete( $preset_name ) {
  // Delete the preset.
  db_query("DELETE FROM {mediafront_preset} WHERE name='%s'", $preset_name);

  // Rebuild Theme Registry
  drupal_rebuild_theme_registry();
}

/**
 * Checks to see if another preset is already taken.
 */
function mediafront_preset_name_exists( $preset_name ) {

  // Get the default presets.
  $default_presets = mediafront_preset_get_default_presets();
  
  // See if there is a default preset name.
  if( $default_presets && isset($default_presets[$preset_name]) ) {
    return true; 
  }    
  else { 
    return (bool)db_result( db_query("SELECT COUNT(*) FROM {mediafront_preset} WHERE name='%s'", $preset_name) );
  }
}

/**
 * Verify the syntax of the given prefix name.
 *
 * Borrowed from the user.module.   :)
 */
function mediafront_validate_preset_name($name) {
  if (!$name) {
    return t('You must enter a preset.');
  }
  if( mediafront_preset_name_exists( $name ) ) {
    return t('The preset name %name is already taken.', array('%name' => $name));
  }
  if (strpos($name, ' ') !== FALSE ) {
    return t('The preset name cannot contain a space.');
  }
  if( is_numeric( $name[0] ) ) {
    return t('The preset name cannot begin with a number.');
  }
  if (preg_match('/[^a-z0-9_]/i', $name)) {
    return t('The preset name contains an illegal character.');
  }
  if (drupal_strlen($name) > MEDIAFRONT_PRESET_MAX_LENGTH) {
    return t('The preset name %name is too long: it must be %max characters or less.', array('%name' => $name, '%max' => MEDIAFRONT_PRESET_MAX_LENGTH));
  }
}
