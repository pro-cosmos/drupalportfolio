<?php


function theme_radios_sex($element) {
  $keys = array_keys($element['#options']);
  $type = $element[$keys[0]]['#type'];
 
  // Start wrapper div for the group.
  $output .= '<div class="my-radios">';
 
  foreach ($keys as $key) {
    // Each radios is theme by calling our custom 'my_radio' theme function.
    $output .= theme('radio_sex', $element[$key]);
  }
 
  $output .= '</div>';
  return $output;
}

function theme_radio_sex($element) {
  _form_set_class($element, array('form-radio'));
  $element['#attributes']['class']= 'validate[required] radio';

  $output = '<input type="radio" ';
  $output .= 'id="' . $element['#id'] . '" ';
  $output .= 'name="' . $element['#name'] . '" ';
  $output .= 'value="' . $element['#return_value'] . '" ';
  $output .= (check_plain($element['#value']) == $element['#return_value']) ? ' checked="checked" ' : ' ';
  $output .= drupal_attributes($element['#attributes']) . ' />';
  if (!is_null($element['#title'])) {
    $output = '<label class="option" for="' . $element['#id'] . '">' . $output . ' ' . $element['#title'] . '</label>';
  }
 
  unset($element['#title']);
  // Default 'form_element' theme is uncommented below, which create a DIV wrapper for each item.
  return theme('form_element', $element, $output);
  //return $output;
}



function theme_ori_tooltiplink($id){  
    $out ='';  
    $i = (int)$id;
    
    if($i>0 && variable_get('ori_regform_tooltip_link'.$i, false))
      {
      $link = variable_get('ori_regform_tooltip_link'.$i, '');
      $title = variable_get('ori_regform_tooltip_title'.$i, '');
      $text = variable_get('ori_regform_tooltip_text'.$i, '');
     
      $out.= "<a id='load-local".$i."' class='load-local' href='#loadme".$i."' rel='#loadme".$i."' title='".$title."'><span>".$link."</span></a>";
      $out.= "<div id='loadme".$i."'>".$text."</div>";
      }
      
return $out;      
}

function theme_ori_regform_tooltip_group($group){
$out ='';

if(variable_get('ori_regform_tooltip_count', 0) > 0)
 {
 $row='';   
 for($i=1; $i<=variable_get('ori_regform_tooltip_count', 0);$i++)
   {
    if(variable_get('ori_regform_tooltip_group'.$i, '') == $group && variable_get('ori_regform_tooltip_link'.$i, '')!='')
      { 
      $row.= '<li>'.theme('ori_tooltiplink', $i).'</li>';
      }
   }
$out = '<ul>'. $row .'</ul>';    
}

return  $out;  
}


