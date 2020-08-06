<?php
/**
 * Helper functions.
 */

use Drupal\Core\Entity\Entity\EntityFormMode;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\Core\Link;
use Drupal\pog_multistep\Utility\EntityFormModeHelper;
use Drupal\Component\Utility\NestedArray;
use Drupal\user\entity\User;

/**
 * Helper function whicj return current pog node for user.
 * @return array|\Drupal\Core\Entity\EntityInterface|mixed|null
 */
function pog_multistep_get_current_node() {
  $node = &drupal_static(__FUNCTION__);

  if (!isset($node)) {
    $node_storage = \Drupal::entityTypeManager()->getStorage('node');
    $node_ids = $node_storage->getQuery()
      ->condition('type', 'pog')
      ->condition('uid', $user = \Drupal::currentUser()->id())
      ->range(0, 1)
      ->execute();
    if (!empty($node_ids)) {
      $nid = array_shift($node_ids);
      $node = $node_storage->load($nid);
    }
  }

  return $node;
}

/**
 * Return rendered pog document template.
 */
function pog_multistep_get_template($entity) {
  $render = [
    '#theme' => 'pog_document',
    '#node' => $entity,
    '#context' => _pog_multistep_get_context($entity),
  ];
  return \Drupal::service('renderer')->render($render);
}

/**
 * Custom pog multistep form step 4 alter
 * @param \Drupal\Core\Form\FormStateInterface $form_state
 */
function _pog_multistep_form_step_1_alter(&$form, FormStateInterface $form_state) {
  $form['field_pog_dateemployedsince']['widget'][0]['value']['#placeholder'] = t('DD/MM/YYYY');
  _pog_multistep_field_hide_none($form, 'field_pog_decision_making_how');
  _pog_multistep_field_hide_none($form, 'field_pog_decision_making');
}

/**
 * Custom pog multistep form step 3 alter
 * @param \Drupal\Core\Form\FormStateInterface $form_state
 */
function _pog_multistep_form_step_3_alter(&$form, FormStateInterface $form_state) {
  _pog_multistep_field_hide_none($form, 'field_pog_on_website');

  $form['field_pog_adress_website']['widget']['add_more']['#value'] = 'Add further Website';
  $form['field_pog_tarif_2ja']['widget']['add_more']['#value'] = 'VERSICHERUNGSUNTERNEMEN';
  $form['field_pog_tariff_3ja']['widget']['add_more']['#value'] = 'POOL';

  $form['field_pog_adress_website']['widget']['add_more']['#value'] = 'Weitere Website';

  $form['field_pog_website_needs_sonstige']['#states'] = [
    'visible' => [
      '[name="field_pog_website_needs[4]"]' => ['checked' => TRUE],
      'AND',
      '[name="field_pog_info_channel[Webseite]"]' => ['checked' => TRUE],
    ]
  ];
}

/**
 * Custom pog multistep form step 4 alter
 * @param \Drupal\Core\Form\FormStateInterface $form_state
 */
function _pog_multistep_form_step_4_alter(&$form, FormStateInterface $form_state) {
  $entity = $form_state->getFormObject()->getEntity();

  _pog_multistep_field_hide_none($form, 'field_pog_inf_from_manuf6');
  _pog_multistep_field_hide_none($form, 'field_pog_age');
  _pog_multistep_field_hide_none($form, 'field_pog_family');
  _pog_multistep_field_hide_none($form, 'field_pog_money');
  _pog_multistep_field_hide_none($form, 'field_pog_prof');
  if (!_pog_multistep_check_weiche(POG_MULTISTEP_WEICHE_3B, $entity)) {
    _pog_multistep_hide_group($form, 'group_geeignetheitsprufung');
  }

  if (isset($form['field_pog_inf_from_manuf5_1soft']['widget']['add_more'])) {
    $form['field_pog_inf_from_manuf5_1soft']['widget']['add_more']['#value'] = 'SOFTWARE';
  }

  if (isset($form['field_pog_select_terms']['widget']['add_more'])) {
    $form['field_pog_select_terms']['widget']['add_more']['#value'] = 'VERGLEICHSOFTWARE';
  }
}

/**
 * Custom pog multistep form step 5 alter
 * @param \Drupal\Core\Form\FormStateInterface $form_state
 */
function _pog_multistep_form_step_5_alter(&$form, FormStateInterface $form_state) {
  $entity = $form_state->getFormObject()->getEntity();

  _pog_multistep_field_hide_none($form, 'field_pog_meeting_ja');
  _pog_multistep_field_hide_none($form, 'field_pog_kooper332_4a');

  if (!_pog_multistep_check_weiche(POG_MULTISTEP_WEICHE_1B, $entity)) {
    _pog_multistep_hide_group($form, 'group_an_eigene_mitarbeiter');
    _pog_multistep_hide_group($form, 'group_3_8');
    _pog_multistep_hide_group($form, 'group_vorgehen_zur_uberwachung_d');
  }
  if (!_pog_multistep_check_weiche(POG_MULTISTEP_WEICHE_4B, $entity)) {
    _pog_multistep_hide_group($form, 'group_an_kooperationspartner_nur');
  }
  if (!_pog_multistep_check_weiche(POG_MULTISTEP_WEICHE_3B, $entity)) {
    if (isset($form['field_pog_monitoring_compare3b'])) {
      $form['field_pog_monitoring_compare3b']['#access'] = FALSE;
    }
  }

  $table_name_array = [
    'field_pog_table_lv',
    'field_pog_table_sach',
    'field_pog_table_kranken',
  ];

  foreach ($table_name_array as $tableKey) {
    $form[$tableKey]['widget']['add_more']['add_more_button_pog_table_3_8_']['#value'] = 'Neue Zeile einfügen';

    foreach ($form[$tableKey]['widget'] as $i => $value) {
      if (isset($value['subform']['field_pog_table2s']['widget'][0]['value']['#title'])) {
        $form[$tableKey]['widget'][$i]['subform']['field_pog_table2s']['widget'][0]['value']['#title'] = ' ';
      }

      // Hide/Show Sonstige textfield.
      if (is_numeric($i)) {
        $selector = sprintf('select[name="%s[%d][subform][%s]"]', $tableKey, $i, 'field_pog_table2');
        $field_pog_table2s = &$form[$tableKey]['widget'][$i]['subform']['field_pog_table2s'];
        $field_pog_table2s['#states'] = ['visible' => [$selector => ['value' => 'Sonstige']]];
        $field_pog_table2s['#element_validate'] = ['_pog_multistep_validate_required'];
      }

      $fields_name = ['field_pog_table1', 'field_pog_table2', 'field_pog_table3', 'field_pog_table4'];
      if ($i === 0) {
        foreach ($fields_name as $field_name) {
          if (isset($form[$tableKey]['widget'][$i]['subform'][$field_name]['widget'][0])) {
            $form[$tableKey]['widget'][$i]['subform'][$field_name]['widget'][0]['#required'] = TRUE;
            $form[$tableKey]['widget'][$i]['subform'][$field_name]['widget'][0]['value']['#required'] = TRUE;
          }
          $form[$tableKey]['widget'][$i]['subform'][$field_name]['widget']['#required'] = TRUE;
        }
      }
    }
  }

  // Button title
  if (isset($form['field_pog_emp_pr']['widget']['add_more'])) {
    $form['field_pog_emp_pr']['widget']['add_more']['#value'] = 'Vergleichssoftwareprogramme';
  }
}

/**
 * Custom pog multistep form step 6 alter
 * @param \Drupal\Core\Form\FormStateInterface $form_state
 */
function _pog_multistep_form_step_6_alter(&$form, FormStateInterface $form_state) {
  $entity = $form_state->getFormObject()->getEntity();

  if (!_pog_multistep_check_weiche(POG_MULTISTEP_WEICHE_1B, $entity)) {
    _pog_multistep_hide_group($form, 'group_vergutung');
  }

  if (empty($entity->field_pog_on_website->value)) {
    $form['field_pog_allgemein1_1']['#access'] = FALSE;
    $form['field_pog_allgemein1_2']['#access'] = FALSE;
    $form['field_pog_allgemein1_3']['#access'] = FALSE;
  }

  foreach ($form as $key => $value) {
    if (isset($value['widget'][0]['value']['#title']) && (strpos($value['widget'][0]['value']['#title'], 'Sonstiges') !== false)) {
      $form[$key]['widget'][0]['value']['#title'] = ' ';
    }
  }
}

/**
 * Custom pog multistep form step 7 alter
 * @param \Drupal\Core\Form\FormStateInterface $form_state
 */
function _pog_multistep_form_step_7_alter(&$form, FormStateInterface $form_state) {
  $entity = $form_state->getFormObject()->getEntity();
  if (!_pog_multistep_check_weiche(POG_MULTISTEP_WEICHE_1B, $entity)) {
    if (isset($form['field_pog_learning_what'])) {
      $form['field_pog_learning_what']['#access'] = FALSE;
    }
  }

  _pog_multistep_field_hide_none($form, 'field_pog_learning');
  _pog_multistep_field_hide_none($form, 'field_pog_are_emp_reliable');
  _pog_multistep_field_hide_none($form, 'field_are_emp_qualified');
}

/**
 * Check given type of weiche (condition).
 * @param $weiche_number
 * @param $entity
 * @return bool
 */
function _pog_multistep_check_weiche($weiche_number, $entity) {
  switch ($weiche_number) {
    case POG_MULTISTEP_WEICHE_1B:
      return !empty($entity->field_pog_other_employees->value);

    case POG_MULTISTEP_WEICHE_2B:
      return !empty($entity->field_pog_on_website->value);

    case POG_MULTISTEP_WEICHE_3B:
      if ($values = $entity->field_pog_life->getValue()) {
        $allowed = [3, 4]; // 3|Klassische Lebensversicherung (Rente, Kapitalleben) 4|Fondsleben (Hybride-Produkte, Fonds-LV)
        foreach ($values as $value) {
          if (in_array(reset($value), $allowed)) {
            return TRUE;
          }
        }
      }

    case POG_MULTISTEP_WEICHE_4B:
      return !empty($entity->field_pog_cooperate_with->value);

    default:
      \Drupal::logger('pog_multistep')->error("Try to check undefined weiche '%name'", ['%name' => $weiche_number]);
  }

  return FALSE;
}

/**
 * Helper functin which hide field group from form.
 * @param $form
 * @param $group_name
 */
function _pog_multistep_hide_group(&$form, $group_name) {
  foreach ($form['#fieldgroups'][$group_name]->children as $field_name) {
    $form[$field_name]['#access'] = FALSE;
  }
  unset($form['#fieldgroups'][$group_name]);
}

/**
 * Hide pog node save messages.
 * @param $node
 */
function _pog_multistep_hide_node_messages($node) {
  $t_args = ['@type' => node_get_type_label($node), '%title' => $node->link($node->label())];
  $hidden = [
    t('@type %title has been created.', $t_args),
    t('@type %title has been updated.', $t_args)
  ];

  foreach ($_SESSION['messages']['status'] as $k => $message) {
    if (in_array($message->__toString(), $hidden)) {
      unset($_SESSION['messages']['status'][$k]);
    }
  }

  if (empty($_SESSION['messages']['status'])) {
    unset($_SESSION['messages']['status']);
  }
}

/**
 * Return array of controlled fields for dependee field.
 * @param $field_name
 * @param $form
 * @return array
 */
function _pog_multistep_controlled_fields($field_name, $form) {
  $out = [];
  if (isset($form['#conditional_fields'][$field_name]['dependees'])) {
    foreach ($form['#conditional_fields'][$field_name]['dependees'] as $controlled) {
      $out[$controlled['dependee']] = $controlled['dependee'];
    }
  }
  return $out;
}

/**
 * Custom checking is controlled field was triggered.
 * @param $controlled_values
 * @param $triggered_value
 * @return bool
 */
function _pog_multistep_field_is_triggered($controlled_values, $triggered_value = 1) {
  if (!empty($controlled_values)) {
    foreach ($controlled_values as $k => $value) {
      if (is_scalar($value)) {
        return $value == $triggered_value;
      }
      else {
        if (reset($value) == $triggered_value) {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

/**
 * Remove field _none option.
 * @param $form
 * @param $field_name
 */
function _pog_multistep_field_hide_none(&$form, $field_name) {
  if (isset($form[$field_name])) {
    unset($form[$field_name]['widget']['#options']['_none']);
  }
}


/**
 * Check is field value is empty.
 * @return bool
 */
function _pog_is_empty($field_name, $value) {
  if (!empty($value)) {
    // Check scalar values.
    $value = !is_array($value) ? [$value] : $value;
    return _pog_field_get_value($field_name, $value);
  }
  return TRUE;
}


function _pog_field_get_value($field_name, $value) {
  $keys = ['value', 'target_id'];
  foreach ($value as $k => $v) {
    if (!is_array($v) && in_array($k, $keys)) {
      $field_type = _pog_multistep_get_field_type($field_name);
      switch ($field_type) {
        case 'integer':
        case 'float':
        case 'entity_reference':
        case 'list_string':
        case 'datetime':
          return empty($v);
        default:
          return empty($v) && $v !== '0';
      }
    }
    else {
      return _pog_field_get_value($field_name, $v);
    }
  }
}

/**
 * Get field type
 * @param $field_name
 * @return string
 */
function _pog_multistep_get_field_type($field_name) {
  $cache = &drupal_static(__FUNCTION__, []);
  if (!isset($cache[$field_name])) {
    $fields = \Drupal::service('entity_field.manager')->getFieldDefinitions('node', 'pog');
    $cache[$field_name] = isset($fields[$field_name]) ? $fields[$field_name]->getType() : '';
  }

  return $cache[$field_name];
}

/**
 * Return context data for POG document view.
 * @param $entity
 * @return array
 */
function _pog_multistep_get_context($entity) {
  $context = [];

  // Remove hidden fields from children array.
  $fields = \Drupal::entityManager()->getFieldDefinitions('node', 'pog');
  foreach ($fields as $key => $field) {
    $selected = $not_selected = [];
    switch ($field->getType()) {
      case 'list_string':
        $ids = $entity->get($key)->getValue();
        foreach ($ids as $k => $v) {
          $ids[$k] = $v['value'];
        }
        if ($ids) {
          $all = $entity->getFieldDefinition($key)->getFieldStorageDefinition()->getSetting('allowed_values');
          $not_selected = array_diff($all, array_combine($ids, $ids));
          $selected = array_intersect_key($all, array_combine($ids, $ids));
        }
        break;

      case 'entity_reference':
        if ($count = $entity->get($key)->count()) {
          for ($i = 0; $i < $count; $i++) {
            $entity_ref = $entity->get($key)[$i]->entity;
            $selected[$entity_ref->id()] = $entity_ref->label();
          }
        }
        break;

      case 'entity_reference_revisions':
        $context[$key] = $entity->get($key)->referencedEntities();
        break;
    }

    // Set custom translation for list item labels.
    reset($selected);
    if (in_array($key, ['field_pog_branche', 'field_pog_employees_get'])) {
        foreach ($selected as $k => $v) {
            $selected[$k] = t($v, [], ['context' => $key . ':pog-item'])->render();
        }
    }

    if ($selected) {
      $context[$key] = $selected;
      $context[$key.'__noSonstiges'] = array_diff($selected, ['Sonstiges']);
    }
    if ($not_selected) {
      $context[$key.'__other'] = $not_selected;
    }
  }

  if ($value = $entity->get('field_pog_adress_website')->getValue()) {
    $context['field_pog_adress_website'] = [];
    foreach ($value as $item) {
      $scheme = parse_url($item['value']);
      $search = '/' . preg_quote($scheme['scheme'] . '://', '/') . '/';
      $item['value'] = preg_replace($search, '', $item['value'], 1);
      $url = Url::fromUri('http://' . $item['value']);
      $context['field_pog_adress_website'][] = $url->getUri();
    }
  }


  // List text field;
  $list_text_fields = ['field_pog_website_needs', 'field_pog_cooper_with'];
  foreach ($list_text_fields as $field_name) {
    $context[$field_name] = _pog_multistep_get_field_values($field_name, $entity);
  }

  // Weiche
  $context['weiche1B'] = _pog_multistep_check_weiche(POG_MULTISTEP_WEICHE_1B, $entity);
  $context['weiche2B'] = _pog_multistep_check_weiche(POG_MULTISTEP_WEICHE_2B, $entity);
  $context['weiche4B'] = _pog_multistep_check_weiche(POG_MULTISTEP_WEICHE_4B, $entity);

  // Money.
  $moneyValues = $ids = $entity->get('field_pog_money')->getValue();
  $arr = [
    'min' => PHP_INT_MAX,
    'max' => 0,
    'checked' => [
      '1' => TRUE,
      '2' => TRUE,
      '3' => TRUE,
      '4' => TRUE,
    ],
  ];
  foreach ($moneyValues as $key => $value) {
    $value = $value['value'];
    switch ($value) {
      case 'Bis 25.000 Euro':
        $arr['min'] = $arr['min_format'] = 0;
        if ($arr['max'] === 0) {
          $arr['max'] = 25000;
          $arr['max_format'] = '25.000';
        }
        unset($arr['checked'][1]);
        $value = ['min' => 0, 'max' => '25.000'];
        break;

      case '25.001 Euro bis 65.000':
        if ($arr['min'] > 25000) {
          $arr['min'] = 25000;
          $arr['min_format'] = '25.000';
        }
        if ($arr['max'] < 65000) {
          $arr['max'] = 65000;
          $arr['max_format'] = '65.000';
        }
        unset($arr['checked'][2]);
        $value = ['min' => '25.001', 'max' => '65.000'];
        break;

      case '65.000 Euro bis 100.000. Euro':
        if ($arr['min'] > 65000) {
          $arr['min'] = 65000;
          $arr['min_format'] = '65.000';
        }
        if ($arr['max'] < 100000) {
          $arr['max'] = 100000;
          $arr['max_format'] = '100.000';
        }
        unset($arr['checked'][3]);
        $value = ['min' => '65.000', 'max' => '100.000'];
        break;

      case 'Mehr als 100.000 Euro':
        if ($arr['min'] > 100000) {
          $arr['min'] = 100000;
          $arr['min_format'] = '100.000';
        }
        $arr['max'] = $arr['max_format'] = NULL;
        unset($arr['checked'][4]);
        $value = ['min' => '100.000', 'max' => NULL];
        break;
    }
  }

  switch (count($moneyValues)) {
    case 1:
      if (!$value['min']) {
        $context['field_pog_money_text'] = 'bis ' . $arr['max_format'] . ' Euro';
      }
      elseif (!$value['max']) {
        $context['field_pog_money_text'] = 'mit mehr als ' . $arr['min_format'] . ' Euro';
      }
      else {
        $context['field_pog_money_text'] = 'zwischen ' . $arr['min_format'] . ' und ' . $arr['max_format'] . ' Euro';
      }
      break;

    case 2:
      if ($arr['max'] !== NULL) {
        $context['field_pog_money_text'] = 'zwischen ' . $arr['min_format'] . ' - ' . $arr['max_format'] . ' Euro';
      }
      else {
        $context['field_pog_money_text'] = 'ab ' . $arr['min_format'] . ' und mehr';
      }
      break;

    case 3:
      $arr['checked'] = reset($arr['checked']);
      switch ($arr['checked']) {
        case 1:
          $context['field_pog_money_text'] = 'bis 25.000 EUR alle Einkommensklassen';
          break;
        case 2:
          $context['field_pog_money_text'] = '25.000 EURO bis 65.000 EUR alle Einkommensklassen';
          break;
        case 3:
          $context['field_pog_money_text'] = '65.000 EURO bis 100.000 EUR alle Einkommensklassen';
          break;
        case 4:
          $context['field_pog_money_text'] = 'mehr als 100.000 EUR alle Einkommensklassen';
          break;
      }
      break;

    case 4:
      $context['field_pog_money_text'] = NULL;
      break;
  }
  $context['field_pog_money_count'] = count($moneyValues);

  // field_pog_learning_whatdoc.
  if (isset($context['field_pog_learning_whatdoc']['Maklerpool, wenn ja, vom welchen Pool'])) {
    unset($context['field_pog_learning_whatdoc']['Maklerpool, wenn ja, vom welchen Pool']);
    $value = $entity->get('field_pog_learning_whatdoc2')->getValue();
    isset($value[0]['value']) ? $context['field_pog_learning_whatdoc']['maklerpool'] = $value[0]['value'] : FALSE;
  }

  if (isset($context['field_pog_learning_whatdoc']['Datenbank eines Versicherungsunternehmen, wenn ja, vom welchen Versicherungsunternehmen'])) {
    unset($context['field_pog_learning_whatdoc']['Datenbank eines Versicherungsunternehmen, wenn ja, vom welchen Versicherungsunternehmen']);
    $value = $entity->get('field_pog_learning_whatdoc1')->getValue();
    isset($value[0]['value']) ? $context['field_pog_learning_whatdoc']['datenbank'] = $value[0]['value'] : FALSE;
  }

  if (isset($context['field_pog_learning_whatdoc']['Sonstiges'])) {
    unset($context['field_pog_learning_whatdoc']['Sonstiges']);
    $value = $entity->get('field_pog_learning_whatdoc_sonst')->getValue();
    isset($value[0]['value']) ? $context['field_pog_learning_whatdoc']['sonstiges'] = $value[0]['value'] : FALSE;
  }

  $field_names = ['field_pog_table_lv', 'field_pog_table_sach', 'field_pog_table_kranken'];
  foreach ($field_names as $field_name) {
    foreach ($context[$field_name] as $i => $field_value) {
      /** @var $field_value \Drupal\paragraphs\Entity\Paragraph */
      $value = $field_value->get('field_pog_table1')->getValue();
      if (empty($value)) {
        unset($context[$field_name][$i]);
      }
    }
  }

  return $context;
}

/**
 * Return multiply field clean values array.
 * @param $field_name
 * @param $entity
 * @return array
 */
function _pog_multistep_get_field_values($field_name, $entity) {
  $out = [];
  if ($value = $entity->get($field_name)->getValue()) {
    foreach ($value as $item) {
      $out[$item['value']] = $item['value'];
    }
  }
  return $out;
}

/**
 * Check if fields was triggered.
 */
function _pog_multistep_dependee_field_triggered($field_name, $form, $form_state) {
  $triggered = FALSE;
  if (isset($form['#conditional_fields'])) {
    $element = $form[$field_name];
    $dependent = $form['#conditional_fields'][reset($element['#array_parents'])];
    // Check if this field's dependencies were triggered.
    $triggered = conditional_fields_evaluate_dependencies($dependent, $form, $form_state);

    // Automatically check if field was triggered.
    if (!empty($dependent['dependees'])) {
      if (count($dependent['dependees']) == 1) {
        $dependee = reset($dependent['dependees']);
        $controlled_field = $dependee['dependee'];
        $value = _pog_multistep_get_dependee_value($dependee);
        $triggered = _pog_multistep_field_is_triggered($form_state->getValue($controlled_field), $value);
      }
      else {
        $first_dependee = current($dependent['dependees']);
        $sign = $first_dependee['options']['grouping'];
        $triggered = ($sign == 'AND');
        foreach ($dependent['dependees'] as $dependee) {
          $controlled_field = $dependee['dependee'];
          $value = _pog_multistep_get_dependee_value($dependee);
          // Check dependee with given sign.
          if ($sign == 'AND') {
            $triggered = $triggered && _pog_multistep_field_is_triggered($form_state->getValue($controlled_field), $value);
          }
          else { // OR
            $triggered = $triggered || _pog_multistep_field_is_triggered($form_state->getValue($controlled_field), $value);
          }
        }
      }
    }
  }

  return $triggered;
}

/**
 * Get field dependee compared value.
 * @param $dependee
 * @return int|string
 */
function _pog_multistep_get_dependee_value($dependee) {
  $value = $dependee_value = '';
  if (isset($dependee['options']['value_form'])) {
    $dependee_value = $dependee['options']['value_form'];
  }

  if ($dependee_value) {
    $value = $dependee_value['value'];
  }
  if (isset($dependee_value[0]['value'])) {
    $value = $dependee_value[0]['value'];
  }

  // fix conditinal field bug: if field visible when checked then value =1
  if (isset($dependee['options']['state']) && $dependee['options']['state'] == 'visible' &&
    isset($dependee['options']['condition']) && $dependee['options']['condition'] == 'checked'
  ) {
    $value = 1;
  }

  return $value;
}

/**
 * Return correct list of visible fields on the form.
 * @param $form
 * @return array
 */
function _pog_multistep_get_form_children($form) {
  $exclude_fields = ['revision', 'revision_log', 'footer', 'path', 'advanced', 'meta'];
  $exclude_types = ['hidden', 'vertical_tabs', 'details', 'markup', 'actions'];
  $result = [];
  foreach (\Drupal\Core\Render\Element::getVisibleChildren($form) as $name) {
    if (!in_array($name, $exclude_fields) && isset($form[$name]['#type']) && !in_array($form[$name]['#type'], $exclude_types)) {
      $result[$name] = $name;
    }
  }

  return $result;
}

/**
 * Finds name of previous form mode sorted by tab index.
 *
 * @param string $op
 *    Name of entity form mode (form operation).
 *
 * @return string|null
 *    Name of previous form mode or NULL if no prevoius operation found.
 */
function _pog_multistep_get_prev_op($op) {
  $cur_index = _pog_multistep_get_tab_index($op);

  if ($cur_index === NULL) {
    return NULL;
  }

  $tab_modes = EntityFormModeHelper::getTabModes();
  _pog_multistep_array_desparse($tab_modes);
  $prev_mode = NULL;

  // Iterate over modes array, trying to find prev mode
  for ($prev_index = $cur_index - 1; $prev_index > -1; $prev_index--) {
    if ($tab_modes[$prev_index] !== NULL) {
      $prev_mode = $tab_modes[$prev_index];
      break;
    }
  }

  if ($prev_mode === NULL) {
    return NULL;
  }

  return EntityFormModeHelper::getOperationName($prev_mode);
}

/**
 * Finds name of next form mode sorted by tab index.
 *
 * @param string $op
 *    Name of entity form mode (form operation).
 *
 * @return string|null
 *    Name of next form mode or NULL if no next operation found.
 */
function _pog_multistep_get_next_op($op) {
  $ops = &drupal_static(__FUNCTION__);

  if (!isset($ops[$op])) {
    $cur_index = _pog_multistep_get_tab_index($op);

    if ($cur_index === NULL) {
      return NULL;
    }

    $tab_modes = EntityFormModeHelper::getTabModes();
    _pog_multistep_array_desparse($tab_modes);
    $num_tab_modes = count($tab_modes);
    $next_mode = NULL;

    // Iterate over modes array, trying to find prev mode
    for ($next_index = $cur_index + 1; $next_index < $num_tab_modes; $next_index++) {
      if ($tab_modes[$next_index] !== NULL) {
        $next_mode = $tab_modes[$next_index];
        break;
      }
    }

    if ($next_mode === NULL) {
      return NULL;
    }

    $ops[$op] = EntityFormModeHelper::getOperationName($next_mode);
  }

  return $ops[$op];
}

/**
 * @param string $op
 *    Entity form mode (operation) machine name.
 *
 * @return int|null
 *    Tab index of $op or NULL on error.
 */
function _pog_multistep_get_tab_index($op) {
  $mode_id = "node.$op";
  $modes = \Drupal::entityTypeManager()->getStorage('entity_form_mode')
    ->loadMultiple();

  if (empty($modes) || !isset($modes[$mode_id])) {
    return NULL;
  }

  $mode = $modes[$mode_id];
  $tab_index = (int) $mode->getThirdPartySetting('pog_multistep', 'tab_index');

  return $tab_index;
}

/**
 * @see http://snipplr.com/view/3803/dealing-with-sparse-arrays/
 *
 * @param $array
 * @param null $filler
 */
function _pog_multistep_array_desparse(&$array, $filler = NULL) {
  $max = -1;

  for (end($array); $key = key($array); prev($array)) {
    if (is_int($key) and $key > $max) {
      $max = $key;
    }
  }

  for ($i = 0; $i <= $max; $i++) {
    if (!array_key_exists($i, $array)) {
      $array[$i] = $filler;
    }
  }

  ksort($array);
}


function _pog_multistep_entity_form_mode_form_alter(&$form, FormStateInterface $form_state) {
  // Add entity builder.
  $form['#entity_builders'][] = 'pog_multistep_entity_form_mode_builder';

  // Add field to form.
  /** @var EntityFormMode $mode */
  $mode = $form_state->getFormObject()->getEntity();
  $tab_index = (int) $mode->getThirdPartySetting('pog_multistep', 'tab_index', 0);

  $form['pog_multistep_tab_index'] = [
    '#type' => 'number',
    '#title' => t('Tab index'),
    '#description' => t("Value of 0 means that this mode won't be used as a tab."),
    '#min' => 0,
    '#step' => 1,
    '#default_value' => $tab_index,
  ];
}

/**
 * Set error style field.
 */
function _pog_multistep_field_set_error(&$form, $form_state, $field_name) {
  $form_state->setError($form[$field_name], t('@name field is required.', ['@name' => $form[$field_name]['#title']]));
  $form[$field_name]['#attributes']['class'][] = 'error';
  $form[$field_name]['#attributes']['class'][] = 'has-error';
  $form[$field_name]['widget']['#attributes']['class'][] = 'error';
}

/**
 * Validate conditional fields in grids field_pog_table_lv, field_pog_table_sach ,field_pog_table_kranken.
 */
function _pog_multistep_validate_required(&$element, FormStateInterface $form_state) {
  $error = FALSE;
  switch ($element['widget']['#field_name']) {
    case 'field_pog_table2s':
      if ($parents = $element['widget'][0]['value']['#parents']) {
        $dependee_value = NestedArray::getValue($form_state->getValues(), $parents);
        $parents[3] = 'field_pog_table2';
        $controled_value = NestedArray::getValue($form_state->getValues(), $parents);
        if ($controled_value == 'Sonstige' && empty($dependee_value)) {
          $error = TRUE;
        }
      }
      break;
  }

  if ($error) {
    $form_state->setError($element, t('@name field is required.', ['@name' => $element['widget']['#title']]));
    $element['#attributes']['class'][] = 'error';
    $element['#attributes']['class'][] = 'has-error';
    $element['widget']['#attributes']['class'][] = 'error';
  }
}

/**
 * Set status to given tab.
 * @param $tab
 * @param $status
 */
function _pog_multistep_set_tab_status($tab, $status, $node = false) {
  // Exit if empty tab.
  if (!$tab){
    return;
  }

  if (!$node) {
    $node = pog_multistep_get_current_node();
  }

  if ($node) {
    $user = User::load(\Drupal::currentUser()->id());
    $userData = \Drupal::service('user.data');
    $tabs_statuses = $userData->get('pog_multistep', $user->id());
    $tabs_statuses = empty($tabs_statuses['tabs_statuses']) ? [] : $tabs_statuses['tabs_statuses'];
    $tabs_statuses[$tab] = $status;
    $userData->set('pog_multistep', $user->id(), 'tabs_statuses', $tabs_statuses);
  }
}

/**
 * Update statuses of tabs which goes after the current tab.
 * @param $cur_op
 * @param $node
 */
function _pog_multistep_update_next_tabs_statuses($cur_op, $node) {
  $tab_modes = EntityFormModeHelper::getTabModes();
  _pog_multistep_array_desparse($tab_modes);
  $is_next_tab = FALSE;
  foreach ($tab_modes as $tab) {
    if ($is_next_tab || $tab && $tab->id() == 'node.' . $cur_op) {
      if ($is_next_tab) {
        $tab_key = str_replace('node.', '', $tab->id());
        _pog_multistep_set_tab_status($tab_key, POG_MULTISTEP_TAB_CHANGED, $node);
      }
      $is_next_tab = TRUE;
    }
  }
}

/**
 * Return list of current tab statuses.
 * @return array|mixed
 */
function _pog_multistep_get_tabs_statuses() {
  $user = User::load(\Drupal::currentUser()->id());
  $userData = \Drupal::service('user.data');
  $tabs_statuses = $userData->get('pog_multistep', $user->id());
  return empty($tabs_statuses['tabs_statuses']) ? [] : $tabs_statuses['tabs_statuses'];
}

/**
 * Get user pog form first not validated tab.
 * @return int|string
 */
function _pog_multistep_get_user_not_validated_tab() {
  $user = User::load(\Drupal::currentUser()->id());
  $userData = \Drupal::service('user.data');
  if ($tabs_statuses = $userData->get('pog_multistep', $user->id())) {
    $tabs_statuses = reset($tabs_statuses);
    foreach ($tabs_statuses as $tab => $status) {
      if ($status != POG_MULTISTEP_TAB_VALIDATED) {
        return $tab;
      }
    }
  }
}