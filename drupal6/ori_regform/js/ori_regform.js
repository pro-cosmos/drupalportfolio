Drupal.behaviors.ori_Regform = function(context) {

    settings = Drupal.settings.ori_Regform ;
    baseurl = settings.baseurl;
    potherKey = 3;

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

   if (typeof ORI_REGFORM_COUNTRY_CODES[currcountry] !=="undefined")
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
     else{
    //forse clear postindex field
    $('#edit-index').val('');
    $('#edit-index-hidden').val('');

   }



   //OnSubmit
   $('#ori-regform-regform').submit(function(){


        //purpose checkboxes must be checked
        if (typeof $('input[name=purposeType]:checked').val() == "undefined"){
          //hide
          $('div.purpose-wrapperformError').remove();
          //show promt
          $("#purpose-wrapper").validationEngine('showPrompt', 'Вы должны выбрать вариант','error','centerRight',true);
          return false;
        }
        else {
          //hide promt
          $('div.purpose-wrapperformError').remove();
        }



       //сохраняем в скрытые поля тк disabled поля не передаются
        $('#edit-communityType-hidden').val($("#tr_communityType input[type=radio]:checked").val());
        // save state value
        $('#edit-state-hidden').val( $("select#edit-state").val() );
        // empty select
        $("select#edit-state").val('');
    return true;
    })

   $("input[name=purposeType]").blur(function(){
   var checked = (typeof $('input[name=purposeType]:checked').val() != "undefined");
   if (checked && $('div.purpose-wrapperformError').length>0){
        $('div.purpose-wrapperformError').remove();
     }
 });

}

function initPurposeType(){
  var checked = (typeof $('input[name=purposeType]:checked').val() != "undefined");
  if (!checked){
    $('#tr_pother').removeClass('hidden').addClass('hidden');
  }

  $('input[name=purposeType]').click(function (){
    if ($('input[name=purposeType]:checked').val() == 3){
      $('#tr_pother').removeClass('hidden');
    }
    else {
      $('#tr_pother').removeClass('hidden').addClass('hidden');
    }
  });

}

function initDatapicker(){
    var d = new Date();
    $("#edit-birthday" ).datepicker({
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

function initFormvalidate(){
    // binds form submission and fields to the validation engine
    $("#ori-regform-regform").validationEngine();

 $("#choices li").live('click', function(){
         $("#pms-regform-regform").validationEngine('hideAll');
  });
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
      $("#tr_communityType").addClass('txtdisable');
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

        $("#edit-state").attr('disabled','disabled');
        $("#edit-state").empty().append('<option value="">'+t('Any')+'</option>');

       // $('#tr_state').addClass('hidden');
        //$("#tr_communityType").addClass('hidden');
        $('#tr_town').addClass('hidden');
        $('#edit-town-hidden').val('');
        $('#tr_district').addClass('hidden');
        $("#edit-district").val('');
        $('#tr_place').addClass('hidden');
        $('#edit-place-hidden').val('');

        //set phone codes
        var currcode = ORI_REGFORM_COUNTRY_CODES[FieldValue];
        $("#edit-tccode").val(currcode);
        $("#edit-mccode").val(currcode);

        //css disable geo form
        if(!$('table.table_inner').hasClass('txtdisable'))
            $('table.table_inner').addClass('txtdisable')
}

   function get_ajax_data(callback_name, postdata, curr_id){


       $.ajax({
                url: '/' + baseurl +"/ajax/" + callback_name,
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
       $("#tr_communityType").removeClass('txtdisable');

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

     //css activate geo form
     $('table.table_inner').removeClass('txtdisable');

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
             else {
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
      $("#tr_communityType").addClass('txtdisable');
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
                url: '/' + baseurl +"/ajax/autocomplete",
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
      if(typeof item !== 'undefined')
      {
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
}


   function t(str){
     return Drupal.t(str);
   }


    