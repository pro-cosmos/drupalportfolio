<?php

/**
 * Implementation of hook_ctools_plugin_api().
 */
function ori_regform_base_feature_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => 1);
  }
}
