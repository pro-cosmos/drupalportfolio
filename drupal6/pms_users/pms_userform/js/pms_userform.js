Drupal.behaviors.pms_Userform = function(context) {

    settings = Drupal.settings.pms_Userform ;
    baseurl = settings.baseurl;
    potherKey = 3;
    formid  = '#pms-users-edit-form';
    // todo: make 1 assoc array from all field caches
    //cacheField = {};
    cacheIndex = [];
    cacheDistrict = [];
    cachePlace = [];
    cacheTown = [];



    initCommon();

    initCountry();
    initAutocompleteField('index',3,6);
    initState();
    initCommunityType();
    initAutocompleteField('town',1,99);
    initAutocompleteField('district',1,99);
    initAutocompleteField('place',1,99);
    initPurposeType();

    initFormvalidate();
    initDatapicker();
    initCluetip();
}


function initCluetip(){
   $('a.load-local').cluetip({local:true, cursor: 'pointer',  arrows: true});
}

function initCommon(){
   var currcountry = $("edit-country").val();

   if (typeof PMS_USERFORM_COUNTRY_CODES[currcountry] !=="undefined")
     {
        $("#edit-tccode").val(currcode);
        $("#edit-mccode").val(currcode);
     }



   //проверяем текущие значения формы
   if(settings.post_index)
   {
    var item = settings.curr_cacheIndex;
    setFieldCache('index', item);
    selectIndex({'id':settings.post_index}, true);
    $("select#edit-state [value='"+item.state_id+"']").attr("selected", "selected");
   }




   //OnSubmit
   $(formid).submit(function(){
       //сохраняем в скрытые поля тк disabled поля не передаются
        $('#edit-communityType-hidden').val($("#tr_communityType input[type=radio]:checked").val());
        // save state value
        $('#edit-state-hidden').val( $("select#edit-state").val() );
        // empty select
        //$("select#edit-state").val('');
    return true;
    })


}

function initPurposeType(){
   if ($('input[name=purposeType]:checked').val() != potherKey){
       $('#tr_pother').addClass('hidden');
     }

   $('input[name=purposeType]').click(function (){
      if ($('input[name=purposeType]:checked').val() == potherKey){
        $('#tr_pother').removeClass('hidden');
      }
      else {
        $('#tr_pother').addClass('hidden');
      }
  });

}

function initDatapicker(){
    var d = new Date();
    if($("#edit-birthday").length>0)
    {
    $("#edit-birthday").datepicker({
        dateFormat: 'dd/mm/yy',
        changeMonth: true,
        changeYear: true,
        yearRange: '1900:' + d.getFullYear(),
        onChangeMonthYear: function(year, month, datePicker) {
          // Prevent forbidden dates, like 2/31/2012:
          var $t = $(this);
          var day = $t.data('preferred-day') || new Date().getDate();
          var newDate = new Date(month + '/' + day + '/' + year);
          while (day > 28) {
            if (newDate.getDate() == day && newDate.getMonth() + 1 == month && newDate.getFullYear() == year) {
              break;
            } else {
              day -= 1;
              newDate = new Date(month + '/' + day + '/' + year);
            }
          }
          $t.datepicker('setDate', newDate);
        },

        beforeShow: function(dateText, datePicker) {
          // Record the starting date so that
          // if the user changes months from Jan->Feb->Mar
          // and the original date was 1/31,
          // then we go 1/31->2/28->3/31.
          $(this).data('preferred-day', ($(this).datepicker('getDate') || new Date()).getDate());
        }
    });
   }
}

function initFormvalidate(){
    // binds form submission and fields to the validation engine
    $(formid).validationEngine();
}




function initState(){

  if(!settings.post_state)
      $("#edit-state").attr('disabled','disabled');

  $("#edit-state").change(function(){
   selectState($(this).val());
  });

   }

function initCommunityType(){

      $("#tr_communityType input[type=radio]").attr('disabled','disabled');
      //$("#tr_communityType input[type=radio]").removeAttr('checked');


      $("#tr_communityType input[type=radio]").change(function(){

         $("#edit-district").val('');

        // выбран город
        if($(this).val()==0){
            $("#tr_town").removeClass('hidden');
            if($("#edit-town").val() =='' )
                $("#edit-town").val( $("#edit-place").val() );

            $("#tr_district").addClass('hidden');
            $("#tr_place").addClass('hidden');
           }
           else
           { //выбрана деревня
            $("#tr_place").removeClass('hidden');
            if($("#edit-place").val() =='' )
                $("#edit-place").val( $("#edit-town").val() );

            $("#tr_town").addClass('hidden');
            $("#tr_district").removeClass('hidden');
           }
      })
  }

function initCountry(){

   $('#edit-country').change(function () {
         selectCountry($(this).val());
        });
   }

function selectCountry(FieldValue){
   //show postindex field
        $('#tr_index').removeClass('hidden');
        $('#edit-index').val('');

       // $('#tr_state').addClass('hidden');
        //$("#tr_communityType").addClass('hidden');
        $('#tr_town').addClass('hidden');
        $('#tr_district').addClass('hidden');
        $('#tr_place').addClass('hidden');

        //set phone codes
        var currcode = PMS_USERFORM_COUNTRY_CODES[FieldValue];
        $("#edit-tccode").val(currcode).attr('disabled','disabled');
        $("#edit-mccode").val(currcode).attr('disabled','disabled');
}

function get_ajax_data(callback_name, postdata, curr_id){


       $.ajax({
                url: baseurl +"/ajax/" + callback_name,
                dataType: "json",
                type: "POST",
                data: postdata,
                error: function(event, request, settings,error){
                    return false;
                },
                success: function(data) {
                       //clear select
                       $("#edit-state").empty();
                       $("#edit-state").append('<option value="">'+t('Any')+'</option>');

                       //console.log(data.result);
                       //fill select
                       $.map( data.result, function( item ) {
                            //console.log(item.title);
                            var selected = (curr_id == item.id)? 'selected="selected"':''
                            $("#edit-state").append('<option value="'+item.id+'" '+selected+'>'+item.title+'</option>');
                         });

                       //$('select#edit-state option:selected').each(function(){
                       //  this.selected=false;
                       //  });
                      //выделить элементс value = curr_id
                      $("select#edit-state").val(curr_id);
                      $("select#edit-state [value='"+curr_id+"']").attr("selected", "selected");
                }
            });
}


function selectField(Field, item) {
   if(Field == 'index')
       selectIndex(item, false);
   if(Field == 'town')
       selectTown(item);
   if(Field == 'district')
       selectDistrict(item);
   if(Field == 'place')
       selectPlace(item);
   }


function selectState(FieldValue){

     $('#edit-state-hidden').val(FieldValue);

     //show next form element
      $("#tr_communityType").removeClass('hidden');
      $("#tr_communityType input[type=radio]").removeAttr('checked');
      $("#tr_communityType input[type=radio]").removeAttr('disabled');
      $("#tr_communityType input[type=radio]").attr('disabled','');


     $('#tr_town').addClass('hidden');
     $('#tr_district').addClass('hidden');
     $('#tr_place').addClass('hidden');

     $("#edit-district").val('');
     $("#edit-town").val('');
     $("#edit-place").val('');
 }


function selectDistrict(item){
       $('#tr_place').removeClass('hidden');
       $('#edit-place').val('');
       $('#edit-district-hidden').val( item.id );
 }

function selectPlace(item){
            $('#edit-place-hidden').val( item.id );

   }

function selectTown(item){
            $('#edit-town-hidden').val( item.id );
   }

function selectIndex(item, mode){

     var FieldValue = item.id;

     $('#edit-index-hidden').val(FieldValue);
     var country = $('select#edit-country option:selected').val();
     var curr = {};
     var state='-1';
     var city='';
     var place='';
     var district ='';
     var communityType = -1;

     //$('#tr_state').addClass('hidden');
     //$("#tr_communityType").addClass('hidden');
     $("#tr_communityType input[type=radio]").removeAttr('checked');
     $('#tr_town').addClass('hidden');
     $("#tr_communityType input[type=radio]").attr('disabled','disabled');
     $('#tr_district').addClass('hidden');
     $('#tr_place').addClass('hidden');


    // индекс найден
     if(cacheIndex.length>0)
     {
       if (typeof cacheIndex[FieldValue] !=="undefined")
         {
          curr = cacheIndex[FieldValue];
          var state = curr.state_id;
          var communityType = (curr.area.length==0)? 0:1;

           if(communityType==1){
              place = curr.city;
              district = curr.area;
             }
             else{
             city = curr.city;
             city_id = curr.city_id;
             }
        }
     }

     if(!mode){
     get_ajax_data('getselect', {'field':'state', 'country':country}, state);
     }


    //область/край
    if(state){
    $("#tr_state").removeClass('hidden');
    $("#edit-state").removeAttr('disabled');
    $('#edit-state-hidden').val(state);
   }


    //тип населенного пункта
    if(communityType != -1){
      $("#tr_communityType").removeClass('hidden');
      $("#tr_communityType input[type=radio]:eq("+communityType+")").attr({checked: true});
      $("#tr_communityType input[type=radio]").attr('disabled','disabled');
      $('#edit-communityType-hidden').val($("#tr_communityType input[type=radio]:checked").val());
    }

//город
    if(city){
      $("#edit-town").val(city);
      $("#edit-town-hidden").val(city_id);
      $("#tr_town").removeClass('hidden');
    }

    //район
    if(district){
      $("#edit-district").val(district);
      $("#tr_district").removeClass('hidden');
      $("#edit-town-hidden").val('');
    }

    //насел пункт
    if(place){
      $("#edit-place").val(place);
      $("#tr_place").removeClass('hidden');
    }

   }


function initAutocompleteField(Field, MinLength, Maxsize){

        $("#edit-"+Field).autocomplete({
        source: function( request, response) {
            //id of autocomplete textfield
            var fid = this.element.attr('id');
            var country = $('select#edit-country option:selected').val();
            var state = $('select#edit-state option:selected').val();
            var area = $('#edit-district').val();


            $.ajax({
                url: baseurl +"/ajax/autocomplete",
                dataType: "json",
                type: "POST",
                data: {
                    'field' : Field,
                    'country': country,
                    'state': state,
                    'area': area,
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
                        selectField(Field, {'id':newKey});
                       }



                    response( $.map( data.result, function( item ) {

                          setFieldCache(Field, item);

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
                 selectField(Field , ui.item);
                 //console.log(ui.item);
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


function setFieldCache(Field, item){
       // todo apply dinamic variable names like cache{Field}
      // console.log(value);
       if(Field == 'district'){
          if(item!='')
             cacheDistrict[item.id] = item;
          else
             cacheDistrict = [];
      }

       if(Field == 'index'){
          if(item!='')
             cacheIndex[item.id] = item;
          else
             cacheIndex = [];
      }
}


function t(str){
     return Drupal.t(str);
   }


