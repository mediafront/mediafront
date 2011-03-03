<?php
/**
 * Implementation of hook_features_export_options().
 */
function mediafront_features_export_options() {
  $options = array();
  foreach (mediafront_preset_get_presets() as $preset) {
    $options[$preset['name']] = $preset['name'];
  }
  return $options;
}

/**
 * Implementation of hook_features_export().
 */
function mediafront_features_export($data, &$export, $module_name = '') {
  foreach ($data as $preset) {
    $export['features']['mediafront'][$preset] = $preset;
  }
}

/**
 * Implementation of hook_features_export_render().
 */
function mediafront_features_export_render($module_name, $data) {
  $items = array();
  foreach ($data as $key) {
    $preset = mediafront_get_preset($key);
    unset($preset['pid']);
    $items[$key] = $preset;
  }
  $code = "  \$items = ". features_var_export($items, '  ') .";\n";
  $code .= '  return $items;';
  return array('mediafront_default_presets' => $code);
}

/**
 * Implementation of hook_features_revert().
 */
function mediafront_features_revert($module) {
  if ($default_presets = features_get_default('mediafront', $module)) {
    foreach (array_keys($default_presets) as $default_preset) {
       mediafront_preset_delete($default_preset);
    }
  }
}
