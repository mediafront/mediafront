<?php
/**
 * Implements hook_preset_create_form()
 */
function mediafront_preset_create_form() {
  $form = array();

  // Get all of the available players.
  $players = mediafront_get_players();
  $options = array();
  foreach( $players as $name => $player ) {
    $options[$name] = $player['title'];
  }

  $form['preset']['settings']['player'] = array(
    '#type' => 'select',
    '#title' => t('Media Player'),
    '#description' => t('Select which player you would like to use for this preset.'),
    '#options' => $options,
    '#weight' => -8,
  );

  return $form;
}

/**
 * Implements hook_preset_form( $preset )
 *
 * @param <type> $preset
 * @return <type>
 */
function mediafront_preset_form( $preset ) {
  $form = array();
  $form['preview'] = array(
    '#type' => 'fieldset',
    '#title' => t('Player Preview'),
    '#attributes' => array('id' => 'mediafront_admin_preview'),
    '#weight' => -10
  );

  $preset['settings'] = array_merge( $preset['settings'], array(
    'admin' => true,
    'preset' => $preset['name']
  ));

  $form['preview']['player'] = array(
    '#type' => 'item',
    '#markup' => mediafront_get_player($preset['settings'])
  );

  // Get all of the settings for this player.
  $preset['settings'] = preset_get_settings( $preset );

  $form['preset']['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Player Settings'),
    '#weight' => -8,
    '#collapsible' => true,
    '#collapsed' => true
  );

  // Now add the player settings form to this fieldset.
  $form['preset']['settings'] = array_merge( mediafront_player_settings_form( $preset ), $form['preset']['settings'] );

  // Add the player to player settings.
  $form['preset']['settings']['connect'] = array(
    '#type' => 'fieldset',
    '#title' => t('Player to Player'),
    '#collapsible' => true,
    '#collapsed' => true
  );

  $result = preset_get_presets('mediafront');
  $presets = array();
  foreach( $result as $pid => $otherPreset ) {
    if( $otherPreset['name'] != $preset['name'] ) {
      $presets[$otherPreset['name']] = $otherPreset['name'];
    }
  }

  $form['preset']['settings']['connect']['iscontroller'] = array(
    '#type' => 'select',
    '#multiple' => TRUE,
    '#title' => t('Make this preset the Control Bar for'),
    '#description' => t('Select which preset you would like to control using this preset.'),
    '#default_value' => isset($preset['settings']['connect']) ? $preset['settings']['connect']['iscontroller'] : '',
    '#options' => $presets
  );

  $form['preset']['settings']['connect']['isplaylist'] = array(
    '#type' => 'select',
    '#multiple' => TRUE,
    '#title' => t('Make this preset the Playlist for'),
    '#description' => t('Select which preset you would like to play this playlist.'),
    '#default_value' => isset($preset['settings']['connect']) ? $preset['settings']['connect']['isplaylist'] : '',
    '#options' => $presets
  );
  return $form;
}
