(function($) {
  $(function() {
    
    jQuery.media = jQuery.media ? jQuery.media : {};

    // Connecting the media blocks to the player.
    var mediaIndex = 0;
    var media = [];
    var firstMedia = null;
    var mediaplayer = null;

    // Get the previous media node.
    jQuery.media.prevMedia = function() {
      mediaIndex = (mediaIndex > 0) ? (mediaIndex - 1) : (media.length - 1);
      return media[mediaIndex];
    };

    // Get the next media node.
    jQuery.media.nextMedia = function() {
      mediaIndex = (mediaIndex < (media.length - 1)) ? (mediaIndex + 1) : 0;
      return media[mediaIndex];
    };

    // Loads a node by checking to see if it is a full object or not.
    jQuery.media.loadNode = function( node ) {
      if (mediaplayer && node && node.nid) {
        mediaplayer.node.setNode(node);
      }
    };

    // Register for media complete events, and load the next media on completion.
    jQuery.media.onLoaded(jQuery.media.playerId, function( player ) {
      
      // Set the mediaplayer.
      mediaplayer = player;
      
      // Bind the media update for when one media completes.
      player.node.player.display.bind( "mediaupdate", function( event, data ) {
        if( data.type == 'complete' && jQuery.media.hasMedia ) {
          jQuery.media.loadNode(jQuery.media.nextMedia());
        }
      });
      
      // Load the first media.
      jQuery.media.loadNode(firstMedia);
    });

    $(".views-field-nid").each(function(index) {
      var nid = $(this).find("span").text();
      if( !firstMedia ) {
        firstMedia = jQuery.media.nodes[nid];
      }

      // Store the media in an array.
      media.push(jQuery.media.nodes[nid]);

      $(this).parent().css("cursor", "pointer").bind('click', {
        nid:nid,
        index:index
      }, function( event, data ) {
        event.preventDefault();
        mediaIndex = index;
        jQuery.media.loadNode( jQuery.media.nodes[event.data.nid] );
      });
    });

    // Handle when a user clicks on the next button.
    $("#mediaplayer_next").click(function(event) {
      event.preventDefault();
      jQuery.media.loadNode(jQuery.media.nextMedia());
    });

    // Handle when a user clicks on the previous button.
    $("#mediaplayer_prev").click(function(event) {
      event.preventDefault();
      jQuery.media.loadNode(jQuery.media.prevMedia());
    });   
  });
})(jQuery);