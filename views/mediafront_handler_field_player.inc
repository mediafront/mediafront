<?php

/**
 * Field handler to provide simple renderer that turns a URL into a clickable link.
 *
 * @ingroup views_field_handlers
 */
class mediafront_handler_field_player extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();
    $options = array_merge( $options, array(
      'preset' => array('default' => ''),
      'intro' => array('default' => ''),
      'commercial' => array('default' => ''),
      'prereel' => array('default' => ''),
      'media' => array('default' => ''),
      'postreel' => array('default' => ''),
      'preview' => array('default' => '')
    ));
    return $options;
  }

  /**
   * Provide link to the page being visited.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $fields = array('' => t('<None>'));
    foreach ($this->view->display_handler->get_handlers('field') as $field => $handler) {
      if( strpos($field, 'mediafront_player') === FALSE ) {
        if ($label = $handler->label()) {
          $fields[$field] = $label;
        }
        else {
          $fields[$field] = $handler->ui_name();
        }
      }
    }

    $presets = mediafront_preset_get_presets();
    $options = array();
    foreach( $presets as $preset ) {
      $options[$preset['name']] = $preset['name'];
    }

    $form['mediafront'] = array(
      '#type' => 'fieldset',
      '#title' => t('Media Player Settings')
    );

    $form['mediafront']['preset'] = array(
      '#type' => 'select',
      '#title' => t('MediaFront Preset'),
      '#options' => $options,
      '#default_value' => $this->options['preset'],
      '#description' => t('Select which preset you would like to use for this field.'),
    );

    $form['mediafront']['mediawrapper'] = array(
      '#type' => 'fieldset',
      '#title' => t('Media Settings')
    );

    $form['mediafront']['mediawrapper']['intro'] = array(
      '#type' => 'select',
      '#title' => t('Intro Media Field'),
      '#options' => $fields,
      '#default_value' => $this->options['intro'],
      '#description' => t('The intro is the media that plays before all other media.'),
    );

    $form['mediafront']['mediawrapper']['commercial'] = array(
      '#type' => 'select',
      '#title' => t('Commercial Media Field'),
      '#options' => $fields,
      '#default_value' => $this->options['commercial'],
      '#description' => t('The commercial media will block out all media controls while it is playing.'),
    );

    $form['mediafront']['mediawrapper']['prereel'] = array(
      '#type' => 'select',
      '#title' => t('Pre-reel Media Field'),
      '#options' => $fields,
      '#default_value' => $this->options['prereel'],
      '#description' => t('The pre-reel plays before the main media.'),
    );

    $form['mediafront']['mediawrapper']['media'] = array(
      '#type' => 'select',
      '#title' => t('Main Media Field'),
      '#options' => $fields,
      '#default_value' => $this->options['media'],
      '#description' => t('The main media.'),
    );

    $form['mediafront']['mediawrapper']['postreel'] = array(
      '#type' => 'select',
      '#title' => t('Post-reel Media Field'),
      '#options' => $fields,
      '#default_value' => $this->options['postreel'],
      '#description' => t('The post-reel plays after the main media.'),
    );

    $form['mediafront']['previewwrapper'] = array(
      '#type' => 'fieldset',
      '#title' => t('Image Settings')
    );

    $form['mediafront']['previewwrapper']['preview'] = array(
      '#type' => 'select',
      '#title' => t('Preview Image Field'),
      '#options' => $fields,
      '#default_value' => $this->options['preview'],
      '#description' => t('The image field to show as the preview image.'),
    );
  }

  function  options_submit($form, &$form_state) {
    parent::options_submit($form, $form_state);
    // Flatten all mediafront options
    $form_state['values']['options']['preset'] = $form_state['values']['options']['mediafront']['preset'];
    $form_state['values']['options'] = array_merge($form_state['values']['options'], $form_state['values']['options']['mediafront']['mediawrapper']);
    $form_state['values']['options'] = array_merge($form_state['values']['options'], $form_state['values']['options']['mediafront']['previewwrapper']);
  }

  // Override the query so that the field is not added.
  function query() {}
  
  // Direct copy and paste from views_handler_field_field.inc get_items routine.
  function renderField( $values, $fieldObj ) {
    $entity = clone $values->_field_data[$fieldObj->field_alias]['entity'];
    $entity_type = $values->_field_data[$fieldObj->field_alias]['entity_type'];
    $langcode = $fieldObj->field_language($entity_type);

    // The field we are trying to display doesn't exist on this entity.
    if (!isset($entity->{$fieldObj->definition['field_name']})) {
      return '';
    }

    $display = array(
      'type' => $fieldObj->options['type'],
      'settings' => $fieldObj->options['settings'],
      'label' => 'hidden',
    );

    $render_array = field_view_field($entity_type, $entity, $fieldObj->definition['field_name'], $display, $langcode);
    return render($render_array[0]);
  }
  
  function render($values) {    
    $options = array(
      'intro' => $this->options['intro'],
      'commercial' => $this->options['commercial'],
      'prereel' => $this->options['prereel'],
      'media' => $this->options['media'],
      'postreel' => $this->options['postreel'],
      'preview' => $this->options['preview']
    );
    
    $node = new stdClass();
    $node->mediafiles = array(
      'media' => array(),
      'images' => array('image' => '')
    );    
    
    foreach( array_filter($options) as $type => $value ) {
      if( isset($this->view->field[$value]) )  {
        $field = $this->view->field[$value];
        
        // Different processing if it is an image.
        if( $field->field_info['type'] == 'image' ) {
          $entity = clone $values->_field_data[$field->field_alias]['entity'];
          $entity_type = $values->_field_data[$field->field_alias]['entity_type'];
          $langcode = $field->field_language($entity_type);
          $value = file_create_url($entity->{$value}[$langcode][0]['uri']);
        }
        else {
          // Render the media field.
          $value = $this->renderField($values, $field);
        }
        
        if( $type == 'preview' ) {
          $node->mediafiles['images']['image'] = $value;
        }
        else {
          $node->mediafiles['media'][$type] = $value;
        }
      }
    }

    return theme('mediafront_field', array('entity' => $node, 'preset' => $this->options['preset']) );
  }
}
