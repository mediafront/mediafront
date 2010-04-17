<?php
/**
 * Menu callback: MediaFront Module administration.
 */
function mediafront_admin_page() {
  $item = menu_get_item();
  if ($content = system_admin_menu_block($item)) {
    $output = theme('admin_block_content', $content);
  }
  else {
    $output = t('You do not have any administrative items.');
  }
  return $output;
}

/**
 * Menu callback: MediaFront Module administration.
 */
function mediafront_presets_overview() {
  $presets = mediafront_preset_get_presets();
  $header = array(t('Preset Name'), t('Description'), array('data' => t('Operations'), 'colspan' => '2'));
  $rows = array(); 
  foreach( $presets as $pid => $preset ) {
      $row = array();
      $row[] = check_plain($preset->name);
      $row[] = array('data' => filter_xss_admin($preset->description), 'class' => 'description');
      $row[] = array('data' => l(t('edit'), 'admin/build/mediafront/preset/manage/'. $preset->name));
      $row[] = array('data' => l(t('delete'), 'admin/build/mediafront/preset/manage/'. $preset->name .'/delete'));
      $rows[] = $row;  
  }
  return theme('table', $header, $rows);
}

function mediafront_preset_form(&$form_state, $preset) {
   $form['preview'] = array(
      '#type' => 'fieldset',
      '#title' => t('Player Preview'),
      '#attributes' => array('id' => 'mediafront_admin_preview'),
      '#weight' => -10
   );

   // Set up the parameters for the player.
   $params = array( 'admin' => true, 'player' => $preset->player, 'preset' => $preset->name );
   $params = $preset->settings ? array_merge( $preset->settings, $params ) : $params;  

   $form['preview']['player'] = array(
      '#type' => 'markup',
      '#value' => mediafront_get_player($params) . '<br/><div class="mediadebug"></div>'
   );

   // Get all of the settings for this player.
   $preset->settings = mediafront_get_settings( $preset->player, $params ); 

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
   $form['settings']['preset']['pid'] = array(
      '#type' => 'value',
      '#value' => $preset->pid,
   );   
   $form['settings']['preset']['name'] = array(
      '#type' => 'value',
      '#value' => $preset->name,
   );   
   $form['settings']['preset']['description'] = array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#description' => t('Add a brief description to your preset name.'),
      '#default_value' => $preset->description,
      '#weight' => -9,
   );
   $form['settings']['preset']['player'] = array(
      '#type' => 'value',
      '#value' => $preset->player,
   );    
   
   $form['settings']['player_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Player Settings'),      
      '#weight' => -8,
      '#collapsible' => true,
      '#collapsed' => true
   );
   
   // Now add the player settings form to this fieldset.
   $form['settings']['player_settings'] = array_merge( mediafront_player_settings_form( $preset ), $form['settings']['player_settings'] );
   
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
      if( $otherPreset->name != $preset->name ) {
         $presets[$otherPreset->name] = $otherPreset->name;
      }
   }

   $form['settings']['connect']['iscontroller'] = array(
      '#type' => 'select',
      '#multiple' => TRUE,
      '#title' => t('Make this preset the Control Bar for'),
      '#description' => t('Select which preset you would like to control using this preset.'),
      '#default_value' => $preset->connect['iscontroller'],
      '#options' => $presets
   );

   $form['settings']['connect']['isplaylist'] = array(
      '#type' => 'select',
      '#multiple' => TRUE,
      '#title' => t('Make this preset the Playlist for'),
      '#description' => t('Select which preset you would like to play this playlist.'),
      '#default_value' => $preset->connect['isplaylist'],
      '#options' => $presets
   );
   
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
 * Submit handler for the preset form.
 */
function mediafront_preset_submit($form, &$form_state) {
  $preset->pid = $form_state['values']['pid'];
  $preset->name = $form_state['values']['name'];
  $preset->description = $form_state['values']['description'];
  $preset->player = $form_state['values']['player']; 
  $preset->connect = array(
     'iscontroller' => $form_state['values']['iscontroller'],
     'isplaylist' => $form_state['values']['isplaylist']
  );
  
  // Now set the player settings that are only different from the defaults.  
  $preset->settings = mediafront_get_params( $preset->player, $form_state['values'], false );

  // Save this preset.
  mediafront_preset_save( $preset );
  $form_state['redirect'] = 'admin/build/mediafront/preset/manage/' . $preset->name;
}

/**
 * Button sumit function: handle the 'Delete' button on the preset form.
 */
function mediafront_preset_delete_submit($form, &$form_state) {
  $preset->name = $form_state['values']['name'];
  $form_state['redirect'] = 'admin/build/mediafront/preset/manage/' . $preset->name . '/delete';
}

/**
 * Menu callback; delete a single preset.
 */
function mediafront_preset_delete_confirm(&$form_state, $preset) {
  $form['name'] = array('#type' => 'value', '#value' => $preset->name);
  $message = t('Are you sure you want to delete the preset %name?', array('%name' => $preset->name));
  $caption .= '<p>' . t('This action cannot be undone.') . '</p>';
  return confirm_form($form, $message, 'admin/build/mediafront/preset', $caption, t('Delete'));
}

/**
 * Execute preset deletion
 */
function mediafront_preset_delete_confirm_submit($form, &$form_state) {
  if( $form_state['values']['confirm'] ) {
    mediafront_preset_delete($form_state['values']['name']);
  }

  $form_state['redirect'] = 'admin/build/mediafront/preset';
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
  mediafront_preset_save($preset);
  $form_state['redirect'] = 'admin/build/mediafront/preset/manage/' . $preset['name'];
}

?>