<?php
 

function subscription_variables_settings(&$form_state){

  $prefix = variable_get('constant_subscription_prefix','');

  $form['new_variable'] = array(
  '#type' => 'textfield',
  '#title' => t('Add new variable'),
  '#default_value' => '',
  '#description'=>t('Enter machine name of variable'),
  '#prefix'=>'<div style="clear:both">'    
   );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add' ),
    '#suffix'=>'</div>'  
  );
  
 $form['table_start'] = array(
    '#type' => 'markup',
    '#value' => "<table class='sticky-enabled tableSelect-processed sticky-table'>"
  );
 
 
  $settings = variable_get($prefix.'_settings_list','');
  if(!empty($settings))
  {
   $i=0;
   foreach($settings as $name=>$attr)
     {
       $i++;

       $form['fieldset'.$i] = array(
          '#title'=>  t(str_replace($prefix.'_', '', $name)), 
          '#type' => 'fieldset',
          '#collapsible' => TRUE,
          '#collapsed' => 1,
          '#prefix'=>'<tr><td>',
          '#suffix'=>'</td>', 
         );

          $form['fieldset'.$i][$name] = array(
          '#type' => 'textarea',
          '#title' => t(str_replace($prefix.'_', '', $name)),
          '#default_value' => variable_get($name,''),
           );

          $form['delete|'.$name] = array(
              '#type' => 'checkbox',
              '#default_value' => '',
              '#title' => t('Delete'),
              '#prefix'=>'<td style="width:60px">',
              '#suffix'=>'</td></tr>', 
            );
    }
 }
 
  $form['table_end'] = array(
    '#type' => 'markup',
    '#value' => "</table>"
  );
  
   
 $form['#submit'][] = 'subscription_variables_settings_submit';

 return system_settings_form($form);
}


/**
 *
 * @param type $form
 * @param type $form_state 
 */
function subscription_variables_settings_submit($form, &$form_state){

    $prefix = variable_get('constant_subscription_prefix','');
    $settings = array();  

  //save settings variable names to array
    foreach($form['#post'] as $name=>$val){
      
      //delete variables
      if(strpos('_'.$name,'delete|')>0){
         $del = explode('|',$name);
         
         variable_del($del[1]);
         unset($settings[$del[1]]);
      }
      elseif(strpos('_'.$name, $prefix)>0)
      { //save variables  
       $settings[$name] = $name; 
      }
      
    }
    
  
  //add new variable
  if(!empty($form['#post']['new_variable']))
     $settings[$prefix.'_'.(string)$form['#post']['new_variable']] = (string)$form['#post']['new_variable']; 

  variable_set($prefix.'_settings_list', $settings);
}
