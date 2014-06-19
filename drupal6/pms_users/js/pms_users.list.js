$(document).ready(function (){
  activate_buttons();
  init_sellAll();
  operations_buttons();
});


function operations_buttons(){
  $('.operations input.form-submit').live('click', function(){
    return confirm("Выполнить операцию '"+$(this).attr('title')+"' ?");
  });
}


function init_sellAll(){

  $('input[name=selAll]').change(function(){
    var all = $(this).attr('checked');
    if(all)
      $('input.idCheckbox').attr("checked","checked");
    else
      $('input.idCheckbox').removeAttr("checked");
  });
}


function activate_buttons(){
  $('.activate-user-sub').click(function(){
    href = $(this).attr('href');
    $.get(href, function(data){
      table = Drupal.parseJson(data)['data'];
      count = Drupal.parseJson(data)['count'];
      $('#activate-table').html(table);
      if (count == 0){
        $('#activate-fieldset').hide();
      }
      activate_buttons();
    });
    return false;
  });
  $('#hide-user-requests').click(function(){
    if (!$('#activate-fieldset').hasClass('collapsed')){
      $('#activate-table').hide('fast', function (){
        $('#activate-fieldset').addClass('collapsed');
      });
    }
    else{
      $('#activate-fieldset').removeClass('collapsed');
      $('#activate-table').show('fast');
    }
    return false;
  });

  $('#subscription-user-filter-form').css('margin-bottom', '0px');

  $('.enable-mails').live('click', function(){
    link = $(this);
    $.get('/admin/subscription/user/activate_mails/' + $(this).attr('userid'), function(data){
      link.parent().html(Drupal.parseJson(data)['link']);
    });
    return false;
  });

  $('.disable-mails').live('click', function(){
    link = $(this);
    $.get('/admin/subscription/user/deactivate_mails/' + $(this).attr('userid'), function(data){
      link.parent().html(Drupal.parseJson(data)['link']);
    });
    return false;
  });
}
