(function($) {
  $(function() {

    // Add the media object.
    jQuery.media = jQuery.media ? jQuery.media : {};

    // Connecting the media blocks to the player.
    var mediaIndex = 0;
    var mediaplayer = null;

    // Get the previous media node.
    jQuery.media.prevMedia = function() {
      mediaIndex = (mediaIndex > 0) ? (mediaIndex - 1) : (jQuery.media.nodes.length - 1);
      return jQuery.media.nodes[mediaIndex];
    };

    // Get the next media node.
    jQuery.media.nextMedia = function() {
      mediaIndex = (mediaIndex < (jQuery.media.nodes.length - 1)) ? (mediaIndex + 1) : 0;
      return jQuery.media.nodes[mediaIndex];
    };

    // Loads a node by checking to see if it is a full object or not.
    jQuery.media.loadNode = function( node ) {
      if (mediaplayer && node && node.nid) {
        mediaplayer.node.setNode(node);
      }
    };

    // Load the next media.
    jQuery.media.loadNext = function() {
      jQuery.media.loadNode(jQuery.media.nextMedia());
    };

    // Load the previous media.
    jQuery.media.loadPrev = function() {
      jQuery.media.loadNode(jQuery.media.prevMedia());
    };

    // Only call this if the code is available.
    if (jQuery.media.onLoaded) {

      // Register for media complete events, and load the next media on completion.
      jQuery.media.onLoaded(jQuery.media.playerId, function( player ) {

        // Set the mediaplayer.
        mediaplayer = player;

        // Bind the media update for when one media completes.
        player.node.player.display.bind( "mediaupdate", function( event, data ) {
          if( data.type == 'complete' && jQuery.media.hasMedia ) {
            jQuery.media.loadNext();
          }
        });

        // Load the first media.
        jQuery.media.loadNode(jQuery.media.nodes[0]);
      });
    }

    // Iterate through all of the nid fields.
    $(jQuery.media.fieldSelector).each(function(index) {
      // Alter the parent handler so that this becomes a link to the main player.
      $(this).parent().css("cursor", "pointer").bind('click', function( event ) {
        event.preventDefault();
        mediaIndex = index;
        jQuery.media.loadNode( jQuery.media.nodes[mediaIndex] );
      });
    });

    // Handle when a user clicks on the next button.
    $("#mediaplayer_next").click(function(event) {
      event.preventDefault();
      jQuery.media.loadNext();
    });

    // Handle when a user clicks on the previous button.
    $("#mediaplayer_prev").click(function(event) {
      event.preventDefault();
      jQuery.media.loadPrev();
    });
  });
}(jQuery));