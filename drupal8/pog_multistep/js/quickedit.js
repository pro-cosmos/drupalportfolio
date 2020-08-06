/**
 * Custom quickedit functional which implement
 * quick editing of the body field.
 **/

(function ($, Drupal) {
  Drupal.behaviors.POGMultistepQuickedit = {
    attach: function attach(context, settings) {
      const quickedit_id = 'data-quickedit-field-id';
      var $fields = $(context).find('[' + quickedit_id + ']');
      if ($fields.length === 0) {
        return;
      }

      var nid = drupalSettings.pog_multistep.nid;

      $('[' + quickedit_id + ']').parent()
        .on('click', function () {
          var entity = Drupal.quickedit.collections.entities.findWhere({entityID: 'node/' + nid});
          entity.set('state', 'launching');
          entity.set('state', 'highlighted');
        });
    }
  };
})(jQuery, Drupal);