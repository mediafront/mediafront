/**
 *  Copyright (c) 2010 Alethia Inc,
 *  http://www.alethia-inc.com
 *  Developed by Travis Tidwell | travist at alethia-inc.com 
 *
 *  License:  GPL version 3.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.

 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */
(function(a){jQuery.media=jQuery.media?jQuery.media:{};jQuery.media.defaults=jQuery.extend(jQuery.media.defaults,{playlist:"",args:[],wildcard:"*"});jQuery.media.ids=jQuery.extend(jQuery.media.ids,{pager:"#mediapager",scroll:"#mediascroll",busy:"#mediabusy",links:"#medialinks"});jQuery.fn.mediaplaylist=function(c,b){if(this.length===0){return null;}return new (function(e,f,d){d=jQuery.media.utils.getSettings(d);this.display=f;var g=this;this.allowResize=true;this.teasers=[];this.selectedTeaser=null;this.activeTeaser=null;this.args=d.args;this.setActive=true;this.activePager=null;this.pager=null;this.parser=jQuery.media.parser(d);this.scrollRegion=f.find(d.ids.scroll).mediascroll(d);this.scrollRegion.clear();this.width=this.scrollRegion.width;this.height=this.scrollRegion.height;if(d.vertical){this.display.width(this.width);}else{this.display.height(this.height);}this.busy=f.find(d.ids.busy);this.busyVisible=false;this.busyImg=this.busy.find("img");this.busyWidth=this.busyImg.width();this.busyHeight=this.busyImg.height();this.links=f.find(d.ids.links).medialinks(d);this.links.loadLinks();this.loading=function(h){if(this.pager){this.pager.enabled=!h;}if(this.activePager){this.activePager.enabled=!h;}if(h){this.busyVisible=true;this.busy.show();}else{this.busyVisible=false;this.busy.hide();}};this.onResize=function(i,h){if(this.allowResize){this.width+=i;this.height+=h;this.pagerWidth+=i;this.scrollRegion.onResize(i,h);if(this.pager){this.pager.display.width(this.width);}this.busy.css({width:this.width,height:this.height});this.busyImg.css({marginLeft:((this.width-this.busyWidth)/2)+"px",marginTop:((this.height-this.busyHeight)/2)+"px"});}};this.addPager=function(h,i){if(h){h.display.bind("loadindex",function(j,k){if(k.active){g.activateTeaser(g.teasers[k.index]);}else{g.selectTeaser(g.teasers[k.index]);}});h.display.bind("loadpage",function(j,k){g.setActive=k.active;g.loadPlaylist({pageIndex:k.index});});if(i&&!this.activePager){this.activePager=h;}}return h;};this.pager=this.addPager(f.find(d.ids.pager).mediapager(d),false);this.links.display.bind("linkclick",function(i,h){g.onLinkClick(h);});this.onLinkClick=function(k){var i=k.index;var j=k.playlist;var h=[];h[i]=k.arg;if(this.pager){this.pager.reset();}if(this.activePager){this.activePager.reset();}this.loadPlaylist({playlist:j,args:h});};this.loadNext=function(){if(this.pager){this.pager.loadNext(true);}else{if(this.activePager){this.activePager.loadNext(true);}}};this.loadPlaylist=function(h){var j={playlist:d.playlist,pageLimit:d.pageLimit,pageIndex:(this.pager?this.pager.activePage:0),args:{}};var i=jQuery.extend({},j,h);this.setArgs(i.args);this.loading(true);if(i.playlist){if(((typeof i.playlist)=="object")){d.playlist=i.playlist.name;this.setPlaylist(i.playlist);}else{if(i.playlist.match(/^http[s]?\:\/\/|\.xml$/i)){this.parser.parseFile(i.playlist,function(k){g.setPlaylist(k);});}else{if(e){e.call(jQuery.media.commands.getPlaylist,function(k){g.setPlaylist(k);},null,i.playlist,i.pageLimit,i.pageIndex,this.args);}}}return true;}return false;};this.setPlaylist=function(k){if(k&&k.nodes){var h=[];jQuery.media.utils.checkVisibility(this.display,h);if(this.pager){this.pager.setTotalItems(k.total_rows);}if(this.activePager){this.activePager.setTotalItems(k.total_rows);}this.scrollRegion.clear();this.resetTeasers();var j=k.nodes.length;for(var i=0;i<j;i++){this.addTeaser(k.nodes[i],i);}this.scrollRegion.activate();if(this.pager){this.pager.loadNext(this.setActive);}if(this.activePager){this.activePager.loadNext(this.setActive);}jQuery.media.utils.resetVisibility(h);}this.loading(false);};this.onVoteSet=function(h){if(h){var k=this.teasers.length;while(k--){var j=this.teasers[k];if(j.node.nodeInfo.nid==h.content_id){j.node.voter.updateVote(h);}}}};this.addTeaser=function(j,h){var i=this.scrollRegion.newItem().mediateaser(e,j,h,d);if(i){i.display.bind("click",i,function(k){g.activateTeaser(k.data);});if(this.activeTeaser){this.activeTeaser.setActive(j.nid==this.activeTeaser.node.nodeInfo.nid);}if(this.selectedTeaser){this.selectedTeaser.setSelected(j.nid==this.selectedTeaser.node.nodeInfo.nid);}this.teasers.push(i);}};this.resetTeasers=function(){var h=this.teasers.length;while(h--){this.teasers[h].reset();}this.teasers=[];};this.refresh=function(){this.scrollRegion.refresh();var h=this.teasers.length;while(h--){this.teasers[h].refresh();}};this.setArgs=function(j){if(j){this.args=d.args;var k=j.length;while(k--){var h=j[k];if(h&&(h!=d.wildcard)){this.args[k]=h;}}}};this.selectTeaser=function(h){if(this.selectedTeaser){this.selectedTeaser.setSelected(false);}this.selectedTeaser=h;if(this.selectedTeaser){this.selectedTeaser.setSelected(true);this.scrollRegion.setVisible(h.index);}};this.activateTeaser=function(h){this.selectTeaser(h);if(this.activeTeaser){this.activeTeaser.setActive(false);}this.activeTeaser=h;if(this.activeTeaser){this.activeTeaser.setActive(true);if(this.pager){this.pager.activeIndex=this.pager.currentIndex=h.index;}if(this.activePager){this.activePager.activeIndex=this.activePager.currentIndex=h.index;}this.display.trigger("playlistload",h.node.nodeInfo);}};})(c,this,b);};})(jQuery);