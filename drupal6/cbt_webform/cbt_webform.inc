<?php

/**
 * Get chart template node by type
 * @param type $component
 */
function cbt_webform_get_select_chart_template($component) {

//get question options a,b,c
  $answers = _cbt_webform_get_options_array($component['extra']['items'], variable_get('chart_labels_crop', 20));

  $data = array();
  $max = 0;
  //get data from submissions
  $res = db_query("SELECT cid, data, count(*) AS answercnt FROM `{webform_submitted_data}` WHERE nid=%d and cid=%d GROUP BY data", $component['nid'], $component['cid']);
  while ($row = db_fetch_array($res)) {
    if (isset($answers[$row['data']])) {
      $data[] = array($answers[$row['data']], $row['answercnt']);
    }

    $max = ($max < (int) $row['answercnt']) ? (int) $row['answercnt'] : $max;
  }

  //if chart data exists
  if ($data) {
    //get chart template
    $chart_node = node_load(variable_get('cbt_webform_chart_select_template_nid', 0));
    $chart_node->data = $data;
    $chart_node->settings['yAxisMaxValue'] = $max + 1;
    $chart_node->settings['numdivlines'] = $max > variable_get('chart_hdiv_max_limit', 9) ? variable_get('chart_hdiv_max_limit', 9) : $max;

    return $chart_node;
  }

  return false;
}

/**
 * Get chart template node by type
 * @param type $component
 */
function cbt_webform_get_grid_chart_template($component) {

//answers 1|yes
  $answers = _cbt_webform_get_options_array($component['extra']['options']);
//questions ques1|where you from?
  $questions = _cbt_webform_get_options_array($component['extra']['questions'], variable_get('chart_labels_crop', 20));
//get array of labels
  $labels = array_merge(array(0 => 0), array_values($questions));
  // dpm($answers);
  //dpm($questions);

  $map = array_keys($questions);
//dpm($map);
//dpm($component);

  $max = 0;
  //get data from submissions
  $res = db_query("SELECT cid, no, data, count(*) AS answercnt FROM `{webform_submitted_data}` WHERE nid=%d and cid=%d GROUP BY no,data", $component['nid'], $component['cid']);
  if ($res) {
    while ($row = db_fetch_array($res)) {
      $arr[] = $row;
      //add next element
      $data[$answers[$row['data']]][array_search($row['no'], $map)] = $row['answercnt'];

      $max = ($max < (int) $row['answercnt']) ? (int) $row['answercnt'] : $max;
    }
  }

  //dpm($arr);
  //dpm($data);
 
  $d = array($labels);
  reset($answers);
  foreach ($answers as $key => $label) {
    $d[$label] = array(0 => $label);

    $i = 0;
    foreach ($answers as $key2 => $label2) {
      $d[$label][] = isset($data[$label][$i]) ? $data[$label][$i] : '0';
      $i++;
    }
  }

  // dpm($d);
//if chart data exists
  if ($d) {
    $d = array_values($d);

    //dpm($d);
    //get chart template
    $chart_node = node_load(variable_get('cbt_webform_chart_grid_template_nid', 0));
    $chart_node->data = $d;
    $chart_node->settings['yAxisMaxValue'] = $max + 1;
    $chart_node->settings['numdivlines'] = $max > variable_get('chart_hdiv_max_limit', 9) ? variable_get('chart_hdiv_max_limit', 9) : $max;

    return $chart_node;
  }

  return false;
}

/**
 *
 * @param type $text
 * @return type
 */
function _cbt_webform_get_options_array($text, $crop=0) {
  $out = array();

  $rows = explode("\n", $text);
  foreach ($rows as $k => $row) {
    list($key, $value) = explode('|', $row);
    //crop if needed
    if ($crop > 0) {
      $value = _cbt_webform_crop_str($value, $crop);
    }


    if ($value != '' && $value != NULL)
      $out[$key] = $value;
  }

  return $out;
}

function _cbt_webform_padleft_str($text, $lenght=0) {
//text-align:right
}

function _cbt_webform_crop_str($string, $limit=100) {

  $string = check_plain(trim($string));
  $lenght = strlen($string);

  $out = '';
  $j = 1;
  for ($i = 0; $i < $lenght; $i++) {
    //current symbol
    $curr = $string[$i];
    //replace _ by <br />
    if ($j >= $limit && $curr == ' ') {
      $curr = '<br />';
      $j = 0;
    }

    $out.=$curr;
    $j++;
  }

  return $out;
}