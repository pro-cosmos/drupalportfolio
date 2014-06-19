$(document).ready(function() {
  if ($('input[name=purposeType]:checked').val() != 4){
    $('#edit-pother-wrapper').parent().parent().hide();
  }
  if ($('input[name=communityType]:checked').val() != 1){
    $('#edit-town-wrapper').parent().parent().hide();
  }
  if ($('input[name=communityType]:checked').val() == 1){
    $('#edit-district-wrapper').parent().parent().hide();
    $('#edit-title-wrapper').parent().parent().hide();
  }

  $('input[name=communityType]').click(function (){
      if ($('input[name=communityType]:checked').val() == 1){
        $('#edit-town-wrapper').parent().parent().show('');
        $('#edit-district-wrapper').parent().parent().hide('');
        $('#edit-district').val('Район');
        $('#edit-title-wrapper').parent().parent().hide('');
        $('#edit-title').val('Название');
      }
      else {
        $('#edit-town-wrapper').parent().parent().hide('');
       // $('#edit-town option:first').attr('selected', 'selected');
        $('#edit-district-wrapper').parent().parent().show('');
        $('#edit-district').val('');
        $('#edit-title-wrapper').parent().parent().show('');
        $('#edit-title').val('');
      }
  });

  $('input[name=purposeType]').click(function (){
      if ($('input[name=purposeType]:checked').val() == 4){
        $('#edit-pother-wrapper').parent().parent().show();
      }
      else {
        $('#edit-pother-wrapper').parent().parent().hide();
      }
  });


  $('#edit-town').change(function () {
    $('#edit-town').children('option[value=-1]').remove();
  });

  $('#edit-birthday-month').change(function () {
    $('#edit-birthday-month').children('option[value=-1]').remove();
  });
	
  $('#edit-birthday-day').change(function () {
    $('#edit-birthday-day').children('option[value=-1]').remove();
  });

  $('#edit-birthday-year').change(function () {
    $('#edit-birthday-year').children('option[value=-1]').remove();
  });

  $('#edit-state').change(function () {
    $('#edit-state').children('option[value=-1]').remove();
    update_selects('town', 'edit-state');
  });

  $('#edit-country').change(function () {
    $('#edit-country').children('option[value=-1]').remove();
    update_selects('state', 'edit-country');
    update_selects('town', 'edit-state');
    code = codes[$('#edit-country').val()];
    if (code == undefined) {
        code = '';
    }
    $('#edit-tccode').val(code);
    $('#edit-mccode').val(code);
  });
  $('#edit-tccode').attr('disabled', 'disabled')
  $('#edit-mccode').attr('disabled', 'disabled')
  $('td.required').append($('<span class="form-required" title="Обязательное поле">*</span>'));
  $('table.user-table > tbody > tr:even').css('background', 'rgb(243, 243, 255)');

  update_selects('state', 'edit-country');
  update_selects('town', 'edit-state');

  set_country_code();
});

function update_selects(type, value_id){
  var values = type =='town' ? stlinks : cslinks;
  var values = values[$('#' + value_id).val()];
  var current_val = $('#edit-' + type).val();
  $('#edit-' + type + ' option').hide();
  if (values != undefined && values != null && values.length > 0){
    for(value in values){
      $('#edit-' + type + ' option[value=' + values[value] + ']').show();
    }
  }
  $('#edit-' + type + ' option').removeAttr('selected');
  if (current_val >= 0 && $.inArray(current_val, values) != -1){
      $('#edit-' + type + ' option[value=' + current_val + ']').attr('selected', 'selected');
  }
  else
  {
    $('#edit-' + type).append('<option value=\'-1\' selected=\'selected\'>');
  }
}

function set_country_code(){
    var country = $('#edit-country').val();
    if (country != -1){
        var code = codes[country];
        $('#edit-tccode').val(code);
        $('#edit-mccode').val(code);
    }
}
