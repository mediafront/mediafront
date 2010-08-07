<?php

/**
 * Adds or updates a mediafront filefield entry
 */
function mediafront_filefield_save( $node_type, $field_name, $media_type, $node_preset, $thumb_preset ) {
  $field = mediafront_filefield_get( $node_type, $field_name );
  $fields = array(
    'node_type' => $node_type,
    'field_name' => $field_name,
    'media_type' => $media_type,
    'node_preset' => $node_preset,
    'thumb_preset' => $thumb_preset
  );
  $fid = (isset($field->fid) && $field->fid) ? 'fid' : array();
  if( $fid ) {
    $fields['fid'] = $field->fid;
  }
  return drupal_write_record('mediafront_filefield', $fields, $fid );
}

/**
 * Deletes a mediafront filefield entry
 */
function mediafront_filefield_delete( $node_type, $field_name ) {
  db_query( "DELETE FROM {mediafront_filefield} WHERE node_type='%s' AND field_name='%s'", $node_type, $field_name );
}

/**
 * Returns a mediafront filefield entry.
 */
function mediafront_filefield_get( $node_type, $field_name ) {
  $sql = "SELECT * FROM {mediafront_filefield} WHERE node_type='%s' AND field_name='%s'";
  return db_fetch_object( db_query( $sql, $node_type, $field_name ) );
}

function mediafront_filefield_get_default( $field, $field_name, $presets ) {
  return ($field && isset($presets[$field->{$field_name}]) ? $presets[$field->{$field_name}] :
         ($field->{$field_name} ? $field->{$field_name} : "mediafront_original"));
}

/**
 * Implementation of hook_form_FORM_ID_alter().
 */
function mediafront_form_content_field_edit_form_alter(&$form, $form_state) {
  // See if this is a FileField.
  if( $form["#field"]["type"] == "filefield" ) {

    // See if this is an ImageField with ImageCache.
    $image = ($form["#field"]["widget"]["type"] == "imagefield_widget") && module_exists('imagecache');

    // If this is a file field, or an image with ImageCache...
    if( ($form["#field"]["widget"]["type"] == "filefield_widget") || $image ) {

      // Create the file field field set.
      $form['mediafront_filefield'] = array(
        "#type" => "fieldset",
        "#weight" => -1,
        "#title" => $image ? t('MediaFront: ImageCache Settings') : t('MediaFront: Media Settings'),
      );

      // Get the file field.
      $field = mediafront_filefield_get( $form['#field']['type_name'], $form['#field']['field_name'] );

      // Preset a different form if we are an image...
      if( $image ) {
        // Get all of the presets.
        $presets = array();
        $pids = array();
        $presets['mediafront_original'] = t('Original Image');
        $presets['none'] = t('None');
        $result = db_query("SELECT * FROM {imagecache_preset}");
        while( $preset = db_fetch_object( $result ) ) {
          $presets[$preset->presetid] = $preset->presetname;
          $pids[$preset->presetname] = $preset->presetid;
        }

        // Preset the media field form.
        $description = t('Select the ImageCache preset that you would like to use for this MediaFront image display.  <strong>Note: The first two selections are special fields which have the following behavior.</strong>');
        $description .= '<br/>';
        $description .= '<ul>';
        $description .= '<li><strong>' . t('Original Image') . '</strong>: ' . t('Only use the original image for the MediaFront Media Player') . '</li>';
        $description .= '<li><strong>' . t('None') . '</strong>: ' . t('Do not include this image field in the MediaFront Media Player') . '</li>';
        $description .= '</ul>';
        
        $form['mediafront_filefield']['preview_image'] = array(
          '#type' => 'select',
          '#title' => t('Media Preview Image'),
          '#description' => $description,
          '#options' => $presets,
          '#default_value' => mediafront_filefield_get_default( $field, 'node_preset', $pids ),
        );

        $form['mediafront_filefield']['thumb_image'] = array(
          '#type' => 'select',
          '#title' => t('Media Thumbnail Image'),
          '#description' => $description,
          '#options' => $presets,
          '#default_value' => mediafront_filefield_get_default( $field, 'thumb_preset', $pids ),
        );
      }
      else {
        // Preset the media field form.
        $description = t('Select how you would like to use this file field for MediaFront.');
        $description .= '<br/>';
        $description .= '<ul>';
        $description .= '<li><strong>' . t('None') . '</strong>: ' . t('Do not include this file field in the MediaFront Media Player') . '</li>';
        $description .= '<li><strong>' . t('Introduction') . '</strong>: ' . t('This selection will make this file field be used as an introduction, before the commercial') . '</li>';
        $description .= '<li><strong>' . t('Commercial') . '</strong>: ' . t('To use this file field as the commercial for this media type') . '</li>';
        $description .= '<li><strong>' . t('Pre-Reel') . '</strong>: ' . t('The pre-reel is shown right before the main media content, but after the commercial.') . '</li>';
        $description .= '<li><strong>' . t('Media Content') . '</strong>: ' . t('The media content is the main media piece to be played to the user.') . '</li>';
        $description .= '<li><strong>' . t('Post-Reel') . '</strong>: ' . t('The post-reel is shown right after the main media content.') . '</li>';
        $description .= '</ul>';

        $form['mediafront_filefield']['media_type'] = array(
          '#type' => 'select',
          '#title' => t('Media Type'),
          '#description' => $description,
          '#options' => array(
            'none' => t('None'),
            'intro' => t('Introduction'),
            'commercial' => t('Commercial'),
            'prereel' => t('Pre-Reel'),
            'media' => t('Media Content'),
            'postreel' => t('Post-Reel')
          ),
          '#default_value' => ($field ? $field->media_type : 'media'),
        );
      }

      $form['#submit'][] = 'mediafront_filefield_submit';
    }
  }
}

/**
 * Implementation of hook_content_fieldapi().
 */
function mediafront_content_fieldapi( $op, $field ) {

  // Delete the mediafront filefield if they delete this field.
  if( $op == 'delete instance' ) {
    mediafront_filefield_delete( $field['type_name'], $field['field_name'] );
  }
}

/**
 * Called when the user submits the FileField form.
 */
function mediafront_filefield_submit( $form, &$form_state ) {
  // SQL to get the image cache preset name.
  $sql = "SELECT presetname FROM {imagecache_preset} WHERE presetid=%d";
  $nid = $form_state['values']['preview_image'];
  $tid = $form_state['values']['thumb_image'];

  // Save this filefield
  mediafront_filefield_save(
    $form_state['values']['type_name'],
    $form_state['values']['field_name'],
    $form_state['values']['media_type'],
    (($nid && is_numeric($nid)) ? db_result(db_query($sql, $nid)) : $nid),
    (($tid && is_numeric($tid)) ? db_result(db_query($sql, $tid)) : $tid)
  );
}

?>