<?php
class ori_views_handler_filter_substitle_in_operator extends views_handler_filter_in_operator {
  var $date_handler = NULL;

  function get_value_options() {
    $result = db_query("SELECT id,title FROM {subscription} group by title");
    while($row = db_fetch_array($result)){
      $options[$row['id']] = $row['title'];    
    }
    $this->value_options = $options;
  }
    
  /**
   * Pass selected date range to render function
   */
  function query() {
   
   $this->ensure_my_table();
    
    if ($this->options['operator'] == 'in') {
        $keys = array_keys($this->value);

      $this->query->add_where(0, $this->table_alias.'.id IN ('. implode(',', $keys).')' );
    }

  }

}