/**
 * @file
 * Custom Backbone overrides.
 */

(function ($, Drupal, drupalSettings, Backbone) {

  'use strict';

  Drupal.quickedit.FieldModel.prototype.trigger = function () {
    // Show reset button after pog document was changed.
    if (arguments[0] == 'change:state' && arguments[2] == 'saved') {
      $('#pog_multistep_button').removeClass('hidden');
    }
    //console.log(arguments,'event');
    // Call original event.
    Backbone.Events.trigger.apply(this, arguments);
  }

}(jQuery, Drupal, drupalSettings, Backbone));


