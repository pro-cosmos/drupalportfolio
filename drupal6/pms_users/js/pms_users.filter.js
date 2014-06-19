Drupal.behaviors.pms_Users_filters = function(context) {

  settings = Drupal.settings.pms_Users_filters ;


  /**
   *  Regnumber form handler class
   */
  var formUserfilters = {
    baseurl : '',

    submit: function () {
    // var regnumber = $('#edit-regnumberconsultant').val();
    //       if(regnumber)
    //         this.pms_regform_ajax('checkregnumber',{'regnumberconsultant':regnumber});

    },
    pms_regform_ajax: function (callback_name, postdata, dtype){
      var dtype = (typeof dtype!=='undefined') ? dtype : 'json';
      $.ajax({
        url: this.baseurl +"/ajax/" + callback_name,
        dataType: dtype,
        type: "POST",
        data: postdata,
        error: function(event, request, settings,error){

          $('#regnumber_exist_message').hide();
          $('#regnumber_not_message').show();

          return false;
        },
        success: function(data) {
          formUserfilters.regform_ajax_success(callback_name, data);
        }
      });
    },

    regform_ajax_success: function (callback_name, data){
      var account = data.result;
      var status = (account !== false)? 1:0;
      $('#edit-regnumber-status').val(status);

      if(status == 0){
        $('#pms_regform_regform_area').show();
        $('#pms_regform_regnumber_area').hide();

        $('.regnumber_exist').hide();
        $('.regnumber_not').show();
      }
      else{
        // $('#pms_regform_regform_area').show();
        $('#pms_regform_regnumber_area').hide();

        $('.regnumber_exist').show();
        $('.regnumber_not').hide();

      }
    },

    initDatapicker: function(fid){
      var d = new Date();
      $("#"+fid ).datepicker({
        dateFormat: 'dd/mm/yy',
        changeMonth: true,
        changeYear: true,
        yearRange: '1900:' + d.getFullYear()
      });
    },

    initAutocompleteField: function(Field, MinLength, Maxsize){
      $("#edit-"+Field).autocomplete({
        source: function( request, response) {
          //id of autocomplete textfield
          var fid = this.element.attr('id');
          var country = $('select#edit-country option:selected').val();
          var state = $('select#edit-state option:selected').val();

          $.ajax({
            url: formUserfilters.baseurl +"/ajax/autocomplete",
            dataType: "json",
            type: "POST",
            data: {
              'field' : Field,
              'country': country,
              'state': state,
              //'area': area,
              'search': request.term
            },
            error: function(event, request, settings,error){
              alert('Ошибка! Сервер не отвечает.');
            },
            success: function( data ) {
              //введеный индекс
              var Key = $('#'+fid).attr('value');
              // если индекс не найден в базе и длинна его = maxsize
              if(data.count == 0 || Key.length >= Maxsize)
              {
                var newKey = Key.substring(0, Maxsize);
                $('#'+fid).val(newKey);

              //setFieldCache(Field, '');
              //selectField(Field, {'id':newKey});
              //alert(newKey);
              //$('#edit-town-hidden').val(newKey);
              }
              else {
               if (data.count == 1) {
                 var item = data.result[0];
                  if (item && Field=='town'){
                    $('#edit-town-hidden').val(item.id);
                  }
                }
              }

              response( $.map( data.result, function( item ) {

                //setFieldCache(Field, item);

                return {
                  label: item.label,
                  value: item.label,
                  id: item.id
                }
              }));

            }
          });
        },
        minLength: MinLength,
        select: function( event, ui ) {
          //selectField(Field , ui.item);
          var selected = ui.item;
          $('#edit-town-hidden').val(selected.id);
        },
        change: function( event, ui ) {
        //console.log(ui.item);
        //alert('change'+$('#'+fid).val());
        },
        open: function() {
        //$( this ).removeClass( "ui-corner-all" ).addClass( "ui-corner-top" );
        },
        close: function() {
        //$( this ).removeClass( "ui-corner-top" ).addClass( "ui-corner-all" );
        // alert($(this).val());
        //console.log();
        }
      });
    }

  };


  formUserfilters.baseurl = settings.baseurl;
  formUserfilters.initDatapicker('edit-regdate-from');
  formUserfilters.initDatapicker('edit-regdate-to');
  formUserfilters.initAutocompleteField('town', 2, 99);
};