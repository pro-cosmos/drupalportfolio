/**
 * Implements custom functional for POG multistep form.
 **/

(function ($, Drupal) {
  Drupal.behaviors.POGMultistepConditions = {
    attach: function attach(context, settings) {

      var Debug = 1;
      var Conditions = {
        '#group-angaben-zu-den-tarifrechne': {
          'sign': 'AND',
          'conditions': [
            {'field': "[name='field_pog_on_website']", 'value': '1'},
            {'field': "[name^='field_pog_info_channel']", 'value': 'Webseite'}
          ]
        }
      };

      var log = function (str, key) {
        if (Debug) {
          key = key || '';
          console.log(str, key);
        }
      }

      var checkForm = function () {

        $.each(Conditions, function (dependend_field, dependent) {
          var sign = dependent.sign;

          var result = true;
          $.each(dependent.conditions, function (k, cond) {
            var controlled_field = $(cond.field);
            log(cond.field, 'check-field');

            var result_field = false;
            $(controlled_field).each(function () {
              var is_checked = $(this).is(':checked');
              if (!result_field && ($(this).val() == cond.value && is_checked)) {
                result_field = true;
                log($(this).val() + '=' + cond.value, 'eq');
              }
            });
            log(result_field, 'result-field');

            result = result && result_field;
          });

          log(result, 'result');

          var $depended = $(dependend_field);
          $depended.hide();
          if (result) {
            $depended.show();
          }

        });
      }

      // Check form conditions for every field changes.
      $('.page-pog form.node-form', context).find('input').on('click change', function (e) {
        //e.stopPropagation()
        checkForm();
      });
      // Check form conditions on form loading.
      $('.page-pog form.node-form').once('pog-form').each(function () {
        checkForm();
      });


    }
  }
})(jQuery, Drupal);