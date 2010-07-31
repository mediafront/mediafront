<?php

/**
 * Adds or updates a mediafront filefield entry
 */
function mediafront_filefield_save( $entity, $bundle, $field_name, $media_type, $node_preset, $thumb_preset ) {
  $field = mediafront_filefield_get( $entity, $bundle, $field_name );
  $fields = array(
    'entity' => $entity,
    'bundle' => $bundle,
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
function mediafront_filefield_delete( $entity, $bundle, $field_name ) {
  db_delete('mediafront_filefield')
    ->condition('entity', $entity)
    ->condition('bundle', $bundle)
    ->condition('field_name', $field_name)
    ->execute();
}

/**
 * Returns a mediafront filefield entry.
 */
function mediafront_filefield_get( $entity, $bundle, $field_name ) {
  return db_select('mediafront_filefield', 'f')
    ->fields('f')
    ->condition('f.entity', $entity)
    ->condition('f.bundle', $bundle)
    ->condition('f.field_name', $field_name)
    ->execute()
    ->fetchObject();
}

/**
 * Implementation of hook_form_FORM_ID_alter().
 */
function mediafront_form_field_ui_field_edit_form_alter(&$form, $form_state) {
  if( ($form["#field"]["type"] == "image") || ($form["#field"]["type"] == "file") ) {

    // See if this is an Image field.
    $image = ($form["#field"]["type"] == "image");

    // Create the file field field set.
    $form['mediafront_filefield'] = array(
      "#type" => "fieldset",
      "#weight" => -1,
      "#title" => $image ? t('MediaFront: Image Settings') : t('MediaFront: Media Settings'),
    );

    // Get the file field.
    $field = mediafront_filefield_get(
      $form['instance']['entity_type']['#value'],
      $form['instance']['bundle']['#value'],
      $form['#field']['field_name']
    );

    // Preset a different form if we are an image...
    if( $image ) {
      // Get all of the image styles.
      $styles = image_styles();
      $options = array();
      $options['mediafront_original'] = t('Original Image');
      $options['none'] = t('None');
      foreach( $styles as $name => $style ) {
        $options[$name] = $style['name'];
      }

      $description = t('Select the Image style that you would like to use for this MediaFront display.  <strong>Note: The first two selections are special fields which have the following behavior.</strong>');
      $description .= '<br/>';
      $description .= '<ul>';
      $description .= '<li><strong>' . t('Original Image') . '</strong>: ' . t('Only use the original image for the MediaFront Media Player') . '</li>';
      $description .= '<li><strong>' . t('None') . '</strong>: ' . t('Do not include this image field in the MediaFront Media Player') . '</li>';
      $description .= '</ul>';

      $form['mediafront_filefield']['node_preset'] = array(
        '#type' => 'select',
        '#title' => t('Media Preview Image'),
        '#description' => $description,
        '#options' => $options,
        '#default_value' => ($field ? $field->node_preset : 'mediafront_original'),
      );

      $form['mediafront_filefield']['thumb_preset'] = array(
        '#type' => 'select',
        '#title' => t('Media Thumbnail Image'),
        '#description' => $description,
        '#options' => $options,
        '#default_value' => ($field ? $field->thumb_preset : 'mediafront_original'),
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

/*
 * Implementation of hook_field_delete_instance().
*/
function mediafront_field_delete_instance($instance) {
  mediafront_filefield_delete( $instance['entity_type'], $instance['bundle'], $instance['field_name'] );
}


/**
 * Called when the user submits the FileField form.
 */
function mediafront_filefield_submit( $form, &$form_state ) {
  // Save this filefield
  mediafront_filefield_save(
    $form_state['values']['instance']['entity_type'],
    $form_state['values']['instance']['bundle'],
    $form_state['values']['instance']['field_name'],
    isset($form_state['values']['media_type']) ? $form_state['values']['media_type'] : '',
    isset($form_state['values']['node_preset']) ? $form_state['values']['node_preset'] : '',
    isset($form_state['values']['thumb_preset']) ? $form_state['values']['thumb_preset'] : ''
  );
}

?>