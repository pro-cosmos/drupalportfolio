<?php


/**
 * Safety load entity wrapper object.
 * @param $entity_type
 * @param $entity
 */
function _mnogo_diff_entity_wrapper($entity_type, $entity) {
  try {
    return entity_metadata_wrapper($entity_type, $entity);
  }
  catch (EntityMetadataWrapperException $exc) {
    watchdog('mnogo_diff', 'Cant load entity wrapper of type %type!' . json_encode($entity), array('%type' => $entity_type), WATCHDOG_WARNING);
  }
}

/**
 * Directly compare two entities.
 */
function _mnogo_diff_compare_entities($entity_original, $entity, $type) {
  $diff = array();
  $entity_info = mnogo_diff_get_entity_info($entity, $type);

  // Get field info.
  $fields_info = field_read_fields(array(
    'entity_type' => $type,
    'bundle'      => $entity_info['bundle']
  ));

  $fields_compare = array_keys($fields_info);
  // Add label field to compare.
  if (isset($entity_info['label'])) {
    $fields_compare[] = $entity_info['label'];
  }

  // Add custom fields.
  if (isset($entity_info['#custom']) && is_array($entity_info['#custom'])) {
    $fields_compare = array_merge($fields_compare, $entity_info['#custom']);
  }

  $wold = _mnogo_diff_entity_wrapper($type, $entity_original);
  $wnew = _mnogo_diff_entity_wrapper($type, $entity);

  if (!empty($wold) && !empty($wnew)) {
    foreach ($wold->getPropertyInfo() as $name => $val) {
      if (in_array($name, $fields_compare)) {
        $val_old = $wold->$name->value();
        $val_new = $wnew->$name->value();

        if (!is_scalar($val_old) || !is_scalar($val_new)) {
          // Field has structured value.
          $val_old = _mnogo_diff_clean_field($fields_info[$name], $val_old);
          $val_new = _mnogo_diff_clean_field($fields_info[$name], $val_new);
        }

        // Clean types and compare if not equal.
        if (json_encode($val_old) != json_encode($val_new)) {
          $vtype = !empty($fields_info[$name]['columns']) ? $fields_info[$name]['columns'] : FALSE;
          $val_old = _mnogo_diff_clean_field_value_type($val_old, $vtype);
          $val_new = _mnogo_diff_clean_field_value_type($val_new, $vtype);
        }

        // Compare values.
        if (json_encode($val_old) != json_encode($val_new)) {
          // Get original field values instead filtered for custom types.
          if ($fields_info[$name]['type'] == 'text_with_summary') {
            $val_old = $wold->$name->value();
            $val_new = $wnew->$name->value();
          }

          $diff[$name] = array('old' => $val_old, 'new' => $val_new);
          _mnogo_diff_field_result_alter($diff, $name, $entity_info);
        }

      }
    }
  }

  // Delete #custom fields from result diff.
  if (isset($entity_info['#custom'])) {
    unset($entity_info['#custom']);
  }
  // Setting up common identification data.
  if (!empty($diff)) {
    $diff += $entity_info;
  }

  return $diff;
}

function _mnogo_diff_field_result_alter(&$diff, $name, $e) {
  $diff_field = $diff[$name];
  $diff_field_changed = isset($diff_field['old']) && isset($diff_field['new']);

  switch ($e['entity_type']) {
    case 'commerce_customer_profile':
      if ($e['bundle'] == 'shipping') {
        if ($name == 'status') {
          $statuses = mnogo_commerce_customer_address_statuses();
          if ($diff_field_changed) {
            $diff[$name . '_title']['old'] = isset($statuses[$diff_field['old']]) ? $statuses[$diff_field['old']] : '';
            $diff[$name . '_title']['new'] = isset($statuses[$diff_field['new']]) ? $statuses[$diff_field['new']] : '';
          }
          else {
            $diff[$name . '_title'] = isset($statuses[$diff_field]) ? $statuses[$diff_field] : '';
          }
        }
      }
      break;
  }
}

/**
 *
 */
function _mnogo_diff_clean_entity($entity, $type) {

  // Setting up common identification data.
  $entity_info = mnogo_diff_get_entity_info($entity, $type);

  // Get field info.
  $fields_info = field_read_fields(array(
    'entity_type' => $type,
    'bundle'      => $entity_info['bundle']
  ));
  $fields_compare = array_keys($fields_info);
  // Add label field to compare.
  if (isset($entity_info['label'])) {
    $fields_compare[] = $entity_info['label'];
  }
  // Add custom fields.
  if (isset($entity_info['#custom']) && is_array($entity_info['#custom'])) {
    $fields_compare = array_merge($fields_compare, $entity_info['#custom']);
  }

  $w = _mnogo_diff_entity_wrapper($type, $entity);
  if (!empty($w)) {
    foreach ($w->getPropertyInfo() as $name => $val) {
      if (in_array($name, $fields_compare)) {
        $value = $w->$name->value();

        if (!is_scalar($value)) {
          // Field has structured value.
          $value = _mnogo_diff_clean_field($fields_info[$name], $value);
        }

        // Clean types.
        $vtype = !empty($fields_info[$name]['columns']) ? $fields_info[$name]['columns'] : FALSE;
        $value = _mnogo_diff_clean_field_value_type($value, $vtype);

        // Get original field values instead filtered for custom types.
        if (isset($fields_info[$name]) && $fields_info[$name]['type'] == 'text_with_summary') {
          $value = $w->$name->value();
        }

        $entity_info[$name] = $value;
        _mnogo_diff_field_result_alter($entity_info, $name, $entity_info);
      }
    }
  }

  // Delete #custom fields from result diff.
  if (isset($entity_info['#custom'])) {
    unset($entity_info['#custom']);
  }

  return $entity_info;
}

/**
 * Cleaning field.
 */
function _mnogo_diff_clean_field($field_info, $field_value, $encode = FALSE) {

  if ($field_info['cardinality'] != 1) {
    // Loop through multiple-value fields.
    foreach ($field_value as $key => $value_item) {
      $value[$key] = _mnogo_diff_clean_field_value($field_info['type'], $value_item);
    }
  }
  else {
    $value = _mnogo_diff_clean_field_value($field_info['type'], $field_value);
  }

  // Remove single 'value' key from value.
  if (isset($value) && is_array($value) && count($value)==1 && isset($value['value'])){
    $value = $value['value'];
  }
  $value = isset($value) ? $value : '';

  return $encode ? json_encode($value) : $value;
}


/**
 * Cleaning single field value.
 */
function _mnogo_diff_clean_field_value($field_type, $value = '') {
  // Convert object type values to array
  $value = is_object($value) ? (array) $value : $value;
  // Get valid array keys to compare
  $valid_keys = _mnogo_diff_field_value_alter($value, $field_type);

  if (is_array($value)) {
    $valid_keys = is_array($valid_keys) ? $valid_keys : array($valid_keys);
    $value = !empty($value) ? array_intersect_key($value, array_flip($valid_keys)) : '';
  }

  return $value;
}

/**
 *  Clean different types of the same field value.
 */
function _mnogo_diff_clean_field_value_type(&$value, $vtype = FALSE) {
  // Check existence of value type settings.
  if (!$vtype) {
    return (string) $value;
  }

  $is_scalar = FALSE;
  if (!is_array($value)) {
    $value = array('value' => $value);
    $is_scalar = TRUE;
  }
  else {
    // Because array keys may be different.
    ksort($value);
  }

  foreach ($value as $k => $v) {
    if (isset($vtype[$k]['type'])) {
      switch ($vtype[$k]['type']) {
        case 'number_decimal':
          $value[$k] = round($v, $vtype['scale']);
          break;
        case 'numeric':
        case 'int':
          $value[$k] = (int) $v;
          break;
      }
    }
  }

  if ($is_scalar) {
    $value = $value['value'];
  }

  return $value;
}

/**
 * Alter field value before filtering.
 */
function _mnogo_diff_field_value_alter(&$value, $field_type) {
  switch ($field_type) {
    case 'node_reference':
      $keys = 'nid';
      break;
    case 'taxonomy_term_reference':
      $keys = array('tid', 'name');
      break;
    case 'image':
      $keys = array('fid', 'alt', 'title');
      _mnogo_diff_field_file_alter($value);
      break;
    case 'file':
      $keys = array('fid', 'uri');
      _mnogo_diff_field_file_alter($value);
      break;
    case 'link_field':
      $keys = array('url', 'title');
      break;
    case 'commerce_price':
      $keys = 'amount';
      break;
    case 'physical_weight':
      $keys = 'weight';
      break;
    case 'physical_dimensions':
      $keys = array('length', 'width', 'height', 'unit');
      break;
    // case 'text_with_summary':
    // Disable its until further notice.
    // _mnogo_diff_field_textarea_alter($value);
    // $keys = 'value';
    // break;
    case 'name':
      $keys = array('given', 'middle', 'family');
      break;
    case 'addressfield':
      $keys = array(
        'country',
        'administrative_area',
        'district',
        'locality',
        'postal_code',
        'thoroughfare',
        'house',
        'flat',
        'no_flat',
        'no_kladr'
      );
      break;
    // Default key name for array entity field.
    default:
      $keys = 'value';
  }

  return $keys;
}

/**
 * Alter file/image field value before comparison.
 */
function  _mnogo_diff_field_file_alter(&$value) {
  if (isset($value['fid'])) {
    $fid = (int) $value['fid'];
    $uri = db_select('file_managed', 'f')->condition('f.fid', $fid, '=')->fields('f', array('uri'))->execute()->fetchField();

    if (!empty($uri)) {
      $value['uri'] = file_create_url($uri);
    }
  }
}

/**
 * Alter big text field value before comparison.
 * Remove all tags and spaces for clear text comparison.
 */
function _mnogo_diff_field_textarea_alter(&$value) {
  if (in_array($value['format'], array('full_html', 'filtered_html'))) {
    $value['value'] = str_replace(' ', '', strip_tags($value['value']));
  }
}