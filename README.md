<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
    "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
<meta http-equiv="Content-Type" content="application/xhtml+xml; charset=UTF-8" />
<meta name="generator" content="AsciiDoc 8.6.9" />
<title>Volmetrics Project</title>
<style type="text/css">
/* Shared CSS for AsciiDoc xhtml11 and html5 backends */

/* Default font. */
body {
  font-family: Georgia,serif;
}

/* Title font. */
h1, h2, h3, h4, h5, h6,
div.title, caption.title,
thead, p.table.header,
#toctitle,
#author, #revnumber, #revdate, #revremark,
#footer {
  font-family: Arial,Helvetica,sans-serif;
}

body {
  margin: 1em 5% 1em 5%;
}

a {
  color: blue;
  text-decoration: underline;
}
a:visited {
  color: fuchsia;
}

em {
  font-style: italic;
  color: navy;
}

strong {
  font-weight: bold;
  color: #083194;
}

h1, h2, h3, h4, h5, h6 {
  color: #527bbd;
  margin-top: 1.2em;
  margin-bottom: 0.5em;
  line-height: 1.3;
}

h1, h2, h3 {
  border-bottom: 2px solid silver;
}
h2 {
  padding-top: 0.5em;
}
h3 {
  float: left;
}
h3 + * {
  clear: left;
}
h5 {
  font-size: 1.0em;
}

div.sectionbody {
  margin-left: 0;
}

hr {
  border: 1px solid silver;
}

p {
  margin-top: 0.5em;
  margin-bottom: 0.5em;
}

ul, ol, li > p {
  margin-top: 0;
}
ul > li     { color: #aaa; }
ul > li > * { color: black; }

.monospaced, code, pre {
  font-family: "Courier New", Courier, monospace;
  font-size: inherit;
  color: navy;
  padding: 0;
  margin: 0;
}
pre {
  white-space: pre-wrap;
}

#author {
  color: #527bbd;
  font-weight: bold;
  font-size: 1.1em;
}
#email {
}
#revnumber, #revdate, #revremark {
}

#footer {
  font-size: small;
  border-top: 2px solid silver;
  padding-top: 0.5em;
  margin-top: 4.0em;
}
#footer-text {
  float: left;
  padding-bottom: 0.5em;
}
#footer-badges {
  float: right;
  padding-bottom: 0.5em;
}

#preamble {
  margin-top: 1.5em;
  margin-bottom: 1.5em;
}
div.imageblock, div.exampleblock, div.verseblock,
div.quoteblock, div.literalblock, div.listingblock, div.sidebarblock,
div.admonitionblock {
  margin-top: 1.0em;
  margin-bottom: 1.5em;
}
div.admonitionblock {
  margin-top: 2.0em;
  margin-bottom: 2.0em;
  margin-right: 10%;
  color: #606060;
}

div.content { /* Block element content. */
  padding: 0;
}

/* Block element titles. */
div.title, caption.title {
  color: #527bbd;
  font-weight: bold;
  text-align: left;
  margin-top: 1.0em;
  margin-bottom: 0.5em;
}
div.title + * {
  margin-top: 0;
}

td div.title:first-child {
  margin-top: 0.0em;
}
div.content div.title:first-child {
  margin-top: 0.0em;
}
div.content + div.title {
  margin-top: 0.0em;
}

div.sidebarblock > div.content {
  background: #ffffee;
  border: 1px solid #dddddd;
  border-left: 4px solid #f0f0f0;
  padding: 0.5em;
}

div.listingblock > div.content {
  border: 1px solid #dddddd;
  border-left: 5px solid #f0f0f0;
  background: #f8f8f8;
  padding: 0.5em;
}

div.quoteblock, div.verseblock {
  padding-left: 1.0em;
  margin-left: 1.0em;
  margin-right: 10%;
  border-left: 5px solid #f0f0f0;
  color: #888;
}

div.quoteblock > div.attribution {
  padding-top: 0.5em;
  text-align: right;
}

div.verseblock > pre.content {
  font-family: inherit;
  font-size: inherit;
}
div.verseblock > div.attribution {
  padding-top: 0.75em;
  text-align: left;
}
/* DEPRECATED: Pre version 8.2.7 verse style literal block. */
div.verseblock + div.attribution {
  text-align: left;
}

div.admonitionblock .icon {
  vertical-align: top;
  font-size: 1.1em;
  font-weight: bold;
  text-decoration: underline;
  color: #527bbd;
  padding-right: 0.5em;
}
div.admonitionblock td.content {
  padding-left: 0.5em;
  border-left: 3px solid #dddddd;
}

div.exampleblock > div.content {
  border-left: 3px solid #dddddd;
  padding-left: 0.5em;
}

div.imageblock div.content { padding-left: 0; }
span.image img { border-style: none; vertical-align: text-bottom; }
a.image:visited { color: white; }

dl {
  margin-top: 0.8em;
  margin-bottom: 0.8em;
}
dt {
  margin-top: 0.5em;
  margin-bottom: 0;
  font-style: normal;
  color: navy;
}
dd > *:first-child {
  margin-top: 0.1em;
}

ul, ol {
    list-style-position: outside;
}
ol.arabic {
  list-style-type: decimal;
}
ol.loweralpha {
  list-style-type: lower-alpha;
}
ol.upperalpha {
  list-style-type: upper-alpha;
}
ol.lowerroman {
  list-style-type: lower-roman;
}
ol.upperroman {
  list-style-type: upper-roman;
}

div.compact ul, div.compact ol,
div.compact p, div.compact p,
div.compact div, div.compact div {
  margin-top: 0.1em;
  margin-bottom: 0.1em;
}

tfoot {
  font-weight: bold;
}
td > div.verse {
  white-space: pre;
}

div.hdlist {
  margin-top: 0.8em;
  margin-bottom: 0.8em;
}
div.hdlist tr {
  padding-bottom: 15px;
}
dt.hdlist1.strong, td.hdlist1.strong {
  font-weight: bold;
}
td.hdlist1 {
  vertical-align: top;
  font-style: normal;
  padding-right: 0.8em;
  color: navy;
}
td.hdlist2 {
  vertical-align: top;
}
div.hdlist.compact tr {
  margin: 0;
  padding-bottom: 0;
}

.comment {
  background: yellow;
}

.footnote, .footnoteref {
  font-size: 0.8em;
}

span.footnote, span.footnoteref {
  vertical-align: super;
}

#footnotes {
  margin: 20px 0 20px 0;
  padding: 7px 0 0 0;
}

#footnotes div.footnote {
  margin: 0 0 5px 0;
}

#footnotes hr {
  border: none;
  border-top: 1px solid silver;
  height: 1px;
  text-align: left;
  margin-left: 0;
  width: 20%;
  min-width: 100px;
}

div.colist td {
  padding-right: 0.5em;
  padding-bottom: 0.3em;
  vertical-align: top;
}
div.colist td img {
  margin-top: 0.3em;
}

@media print {
  #footer-badges { display: none; }
}

#toc {
  margin-bottom: 2.5em;
}

#toctitle {
  color: #527bbd;
  font-size: 1.1em;
  font-weight: bold;
  margin-top: 1.0em;
  margin-bottom: 0.1em;
}

div.toclevel0, div.toclevel1, div.toclevel2, div.toclevel3, div.toclevel4 {
  margin-top: 0;
  margin-bottom: 0;
}
div.toclevel2 {
  margin-left: 2em;
  font-size: 0.9em;
}
div.toclevel3 {
  margin-left: 4em;
  font-size: 0.9em;
}
div.toclevel4 {
  margin-left: 6em;
  font-size: 0.9em;
}

span.aqua { color: aqua; }
span.black { color: black; }
span.blue { color: blue; }
span.fuchsia { color: fuchsia; }
span.gray { color: gray; }
span.green { color: green; }
span.lime { color: lime; }
span.maroon { color: maroon; }
span.navy { color: navy; }
span.olive { color: olive; }
span.purple { color: purple; }
span.red { color: red; }
span.silver { color: silver; }
span.teal { color: teal; }
span.white { color: white; }
span.yellow { color: yellow; }

span.aqua-background { background: aqua; }
span.black-background { background: black; }
span.blue-background { background: blue; }
span.fuchsia-background { background: fuchsia; }
span.gray-background { background: gray; }
span.green-background { background: green; }
span.lime-background { background: lime; }
span.maroon-background { background: maroon; }
span.navy-background { background: navy; }
span.olive-background { background: olive; }
span.purple-background { background: purple; }
span.red-background { background: red; }
span.silver-background { background: silver; }
span.teal-background { background: teal; }
span.white-background { background: white; }
span.yellow-background { background: yellow; }

span.big { font-size: 2em; }
span.small { font-size: 0.6em; }

span.underline { text-decoration: underline; }
span.overline { text-decoration: overline; }
span.line-through { text-decoration: line-through; }

div.unbreakable { page-break-inside: avoid; }


/*
 * xhtml11 specific
 *
 * */

div.tableblock {
  margin-top: 1.0em;
  margin-bottom: 1.5em;
}
div.tableblock > table {
  border: 3px solid #527bbd;
}
thead, p.table.header {
  font-weight: bold;
  color: #527bbd;
}
p.table {
  margin-top: 0;
}
/* Because the table frame attribute is overriden by CSS in most browsers. */
div.tableblock > table[frame="void"] {
  border-style: none;
}
div.tableblock > table[frame="hsides"] {
  border-left-style: none;
  border-right-style: none;
}
div.tableblock > table[frame="vsides"] {
  border-top-style: none;
  border-bottom-style: none;
}


/*
 * html5 specific
 *
 * */

table.tableblock {
  margin-top: 1.0em;
  margin-bottom: 1.5em;
}
thead, p.tableblock.header {
  font-weight: bold;
  color: #527bbd;
}
p.tableblock {
  margin-top: 0;
}
table.tableblock {
  border-width: 3px;
  border-spacing: 0px;
  border-style: solid;
  border-color: #527bbd;
  border-collapse: collapse;
}
th.tableblock, td.tableblock {
  border-width: 1px;
  padding: 4px;
  border-style: solid;
  border-color: #527bbd;
}

table.tableblock.frame-topbot {
  border-left-style: hidden;
  border-right-style: hidden;
}
table.tableblock.frame-sides {
  border-top-style: hidden;
  border-bottom-style: hidden;
}
table.tableblock.frame-none {
  border-style: hidden;
}

th.tableblock.halign-left, td.tableblock.halign-left {
  text-align: left;
}
th.tableblock.halign-center, td.tableblock.halign-center {
  text-align: center;
}
th.tableblock.halign-right, td.tableblock.halign-right {
  text-align: right;
}

th.tableblock.valign-top, td.tableblock.valign-top {
  vertical-align: top;
}
th.tableblock.valign-middle, td.tableblock.valign-middle {
  vertical-align: middle;
}
th.tableblock.valign-bottom, td.tableblock.valign-bottom {
  vertical-align: bottom;
}


/*
 * manpage specific
 *
 * */

body.manpage h1 {
  padding-top: 0.5em;
  padding-bottom: 0.5em;
  border-top: 2px solid silver;
  border-bottom: 2px solid silver;
}
body.manpage h2 {
  border-style: none;
}
body.manpage div.sectionbody {
  margin-left: 3em;
}

@media print {
  body.manpage div#toc { display: none; }
}


/* BEG @import url(bare.css); */
/* ---------------------------------------------------------------------------
   Bare AsciiDoc styles
   Ryan Tomayko <r@tomayko.com>
--------------------------------------------------------------------------- */

body {
	font-family:verdana,helvetica,arial,sans-serif;
	font-size:81.25%;   /* 13px */
	line-height:1.538;  /* 20px */
	margin:40px 50px;
	max-width:56.250em; /* 900px */
	color:#333;
	text-align: justify;
	text-justify: inter-word;
}

em {
	font-style:italic;
}

strong {
	font-weight:bold;
	color:#000;
}

tt {
	font-family:consolas, 'lucida console', 'bitstream vera sans mono',
	           'courier new', monospace;
	color:#000;
}

p, ul, ol, dl {
	margin:10px 0
}

dl {
	margin-left:40px
}

dt {
	font-weight:normal;
	color:#000;
}

h1, h2, h3, h4, h5 {
	font-family:'lucida grande',georgia,verdana,helvetica,arial,sans-serif;
	font-weight:normal;
	color:#000;
}

h1 {
	font-size:30px;
	line-height:1.428;
	margin:20px 0;
}

h2 {
	font-size:23px;
	line-height:1.36363636; /* repeating, of course */
	margin:20px 0;
}

h2 + .sectionbody {}

h3 {
	font-size:18px;
	line-height:1.1;
	margin:30px 0 10px 0;
}

h4 {
	font-size:13px;
	font-weight:bold;
	line-height:1.538;
}

h5 {
	font-size:13px;
	font-style:italic;
	line-height:1.538;
}

pre {
	font-size:larger;
}

#header {
	text-align:center;
	margin-bottom:30px;
}

#header h1 { margin-bottom:0 }


.title, .sidebar-title {
	font-weight:normal;
	color:#000;
	margin-bottom:0;
}

.admonitionblock .title {
	font-weight:bold;
}

.admonitionblock {
	margin:30px 0px;
	color:#555;
}

.admonitionblock td.icon {
	width:30px;
	padding-right:20px;
	padding-left:20px;
	text-transform:uppercase;
	font-weight:bold;
	color:#888;
}

.listingblock .content {
	border:1px solid silver;
	background:#eee;
	padding:5px;
}

.listingblock .content pre {
	margin:0;
}

.literalblock .content {
	margin-left:40px;
}

.verseblock .content {
	white-space:pre
}

.sidebarblock .sidebar-content {
	border:1px solid silver;
	background:#FFFFEE;
	padding:0 10px;
	color:#222;
	font-size:smaller;
	line-height:1.5;
}

.sidebar-title {
	margin:10px 0;
	font-weight:bold;
	color:#442;
}

.quoteblock-content {
	font-style:italic;
	color:#444;
	margin-left:40px;
}

.quoteblock-content .attribution {
	font-style:normal;
	text-align:right;
	color:#000;
}

.exampleblock-content *:first-child { margin-top:0 }
.exampleblock-content {
	border-left:2px solid silver;
	padding-left:8px;
}

#footer {
	font-size:11px;
	margin-top:40px;
	border-top:1px solid silver;
	color:#555;
}

#author {
	color:#000;
	text-transform:uppercase
}

/* vim: set ft=css ts=4 sw=4 noexpandtab: */

/* END @import url(bare.css); */

/* ---------------------------------------------------------------------------
   FreeBSD AsciiDoc Theme
   Ryan Tomayko <r@tomayko.com>

   Based on The FreeBSD Handbook and various other FreeBSD documenration.
--------------------------------------------------------------------------- */

body {
  font-family:verdana,helvetica,arial,sans-serif;
  font-size:100%;
  color:#000;
}

tt { color:#007A00 }
pre tt { color:#000 }

dt { color:#000 }

h1, h2, h3, h4, h5 {
  font-family:'lucida grande',helvetica,verdana,sans-serif;
  color:#900;
  font-weight:bold;
}

#header {
  text-align:left;
}
#header h1 { margin-bottom:40px }

h1 {
  font-size:36px;
  line-height:1;
  margin:40px 0;
}

h2 {
  font-size:28px;
  line-height:1;
  margin:30px 0 20px 0;
}

.sectionbody {
  margin-left:30px;
}

pre {
  background:#EEE;
}

/* vim: set ft=css ts=4 sw=4 noexpandtab: */

/* XXX Mine */
.monospaced, code, pre {
  font-family: "Lucida Console", Monaco, monospace;
  font-size: 14px;
  color: navy;
  padding: 0;
  margin: 0;
}
</style>
<script type="text/javascript">
/*<![CDATA[*/
var asciidoc = {  // Namespace.

/////////////////////////////////////////////////////////////////////
// Table Of Contents generator
/////////////////////////////////////////////////////////////////////

/* Author: Mihai Bazon, September 2002
 * http://students.infoiasi.ro/~mishoo
 *
 * Table Of Content generator
 * Version: 0.4
 *
 * Feel free to use this script under the terms of the GNU General Public
 * License, as long as you do not remove or alter this notice.
 */

 /* modified by Troy D. Hanson, September 2006. License: GPL */
 /* modified by Stuart Rackham, 2006, 2009. License: GPL */

// toclevels = 1..4.
toc: function (toclevels) {

  function getText(el) {
    var text = "";
    for (var i = el.firstChild; i != null; i = i.nextSibling) {
      if (i.nodeType == 3 /* Node.TEXT_NODE */) // IE doesn't speak constants.
        text += i.data;
      else if (i.firstChild != null)
        text += getText(i);
    }
    return text;
  }

  function TocEntry(el, text, toclevel) {
    this.element = el;
    this.text = text;
    this.toclevel = toclevel;
  }

  function tocEntries(el, toclevels) {
    var result = new Array;
    var re = new RegExp('[hH]([1-'+(toclevels+1)+'])');
    // Function that scans the DOM tree for header elements (the DOM2
    // nodeIterator API would be a better technique but not supported by all
    // browsers).
    var iterate = function (el) {
      for (var i = el.firstChild; i != null; i = i.nextSibling) {
        if (i.nodeType == 1 /* Node.ELEMENT_NODE */) {
          var mo = re.exec(i.tagName);
          if (mo && (i.getAttribute("class") || i.getAttribute("className")) != "float") {
            result[result.length] = new TocEntry(i, getText(i), mo[1]-1);
          }
          iterate(i);
        }
      }
    }
    iterate(el);
    return result;
  }

  var toc = document.getElementById("toc");
  if (!toc) {
    return;
  }

  // Delete existing TOC entries in case we're reloading the TOC.
  var tocEntriesToRemove = [];
  var i;
  for (i = 0; i < toc.childNodes.length; i++) {
    var entry = toc.childNodes[i];
    if (entry.nodeName.toLowerCase() == 'div'
     && entry.getAttribute("class")
     && entry.getAttribute("class").match(/^toclevel/))
      tocEntriesToRemove.push(entry);
  }
  for (i = 0; i < tocEntriesToRemove.length; i++) {
    toc.removeChild(tocEntriesToRemove[i]);
  }

  // Rebuild TOC entries.
  var entries = tocEntries(document.getElementById("content"), toclevels);
  for (var i = 0; i < entries.length; ++i) {
    var entry = entries[i];
    if (entry.element.id == "")
      entry.element.id = "_toc_" + i;
    var a = document.createElement("a");
    a.href = "#" + entry.element.id;
    a.appendChild(document.createTextNode(entry.text));
    var div = document.createElement("div");
    div.appendChild(a);
    div.className = "toclevel" + entry.toclevel;
    toc.appendChild(div);
  }
  if (entries.length == 0)
    toc.parentNode.removeChild(toc);
},


/////////////////////////////////////////////////////////////////////
// Footnotes generator
/////////////////////////////////////////////////////////////////////

/* Based on footnote generation code from:
 * http://www.brandspankingnew.net/archive/2005/07/format_footnote.html
 */

footnotes: function () {
  // Delete existing footnote entries in case we're reloading the footnodes.
  var i;
  var noteholder = document.getElementById("footnotes");
  if (!noteholder) {
    return;
  }
  var entriesToRemove = [];
  for (i = 0; i < noteholder.childNodes.length; i++) {
    var entry = noteholder.childNodes[i];
    if (entry.nodeName.toLowerCase() == 'div' && entry.getAttribute("class") == "footnote")
      entriesToRemove.push(entry);
  }
  for (i = 0; i < entriesToRemove.length; i++) {
    noteholder.removeChild(entriesToRemove[i]);
  }

  // Rebuild footnote entries.
  var cont = document.getElementById("content");
  var spans = cont.getElementsByTagName("span");
  var refs = {};
  var n = 0;
  for (i=0; i<spans.length; i++) {
    if (spans[i].className == "footnote") {
      n++;
      var note = spans[i].getAttribute("data-note");
      if (!note) {
        // Use [\s\S] in place of . so multi-line matches work.
        // Because JavaScript has no s (dotall) regex flag.
        note = spans[i].innerHTML.match(/\s*\[([\s\S]*)]\s*/)[1];
        spans[i].innerHTML =
          "[<a id='_footnoteref_" + n + "' href='#_footnote_" + n +
          "' title='View footnote' class='footnote'>" + n + "</a>]";
        spans[i].setAttribute("data-note", note);
      }
      noteholder.innerHTML +=
        "<div class='footnote' id='_footnote_" + n + "'>" +
        "<a href='#_footnoteref_" + n + "' title='Return to text'>" +
        n + "</a>. " + note + "</div>";
      var id =spans[i].getAttribute("id");
      if (id != null) refs["#"+id] = n;
    }
  }
  if (n == 0)
    noteholder.parentNode.removeChild(noteholder);
  else {
    // Process footnoterefs.
    for (i=0; i<spans.length; i++) {
      if (spans[i].className == "footnoteref") {
        var href = spans[i].getElementsByTagName("a")[0].getAttribute("href");
        href = href.match(/#.*/)[0];  // Because IE return full URL.
        n = refs[href];
        spans[i].innerHTML =
          "[<a href='#_footnote_" + n +
          "' title='View footnote' class='footnote'>" + n + "</a>]";
      }
    }
  }
},

install: function(toclevels) {
  var timerId;

  function reinstall() {
    asciidoc.footnotes();
    if (toclevels) {
      asciidoc.toc(toclevels);
    }
  }

  function reinstallAndRemoveTimer() {
    clearInterval(timerId);
    reinstall();
  }

  timerId = setInterval(reinstall, 500);
  if (document.addEventListener)
    document.addEventListener("DOMContentLoaded", reinstallAndRemoveTimer, false);
  else
    window.onload = reinstallAndRemoveTimer;
}

}
asciidoc.install(4);
/*]]>*/
</script>
</head>
<body class="article">
<div id="header">
<h1>Volmetrics Project</h1>
<span id="author">Stathis Kamperis</span><br />
<span id="revdate">Eleni Maria Stea</span>
<div id="toc">
  <div id="toctitle">Table of Contents</div>
  <noscript><p><b>JavaScript must be enabled in your browser to display the table of contents.</b></p></noscript>
</div>
</div>
<div id="content">
<div class="sect1">
<h2 id="_introduction">1. Introduction</h2>
<div class="sectionbody">
<div class="paragraph"><p>The volmetrics project started as an endeavour to create a volumetric medical image software,
in order to study certain geometrical aspects of malignant tumors. We had already done some
prototyping on the matter with Mathematica and R.</p></div>
<div class="paragraph"><p>This ended up being more a DICOM viewer.</p></div>
</div>
</div>
<div class="sect1">
<h2 id="_features">2. Features</h2>
<div class="sectionbody">
<div class="ulist"><ul>
<li>
<p>
Cross platform application (tested in Linux and Mac OSX)
</p>
</li>
<li>
<p>
Makes use of robust free/open source software (<a href="https://www.qt.io">Qt</a> for GUI, <a href="http://dicom.offis.de/dcmtk.php.en">DCMTK</a> for DICOM manipulation)
</p>
</li>
<li>
<p>
OpenGL accelerated graphics operations with vertex shaders
</p>
</li>
<li>
<p>
Multithreading in CPU/IO intensive tasks (e.g. traversing filesystem to find DICOM files),
 so that main thread doesn&#8217;t block and GUI remains responsive to user input
</p>
</li>
<li>
<p>
Display both CT and MRI data sets
</p>
</li>
<li>
<p>
Zoom in/out, pan, flip horizontally/vertically, reset
</p>
</li>
<li>
<p>
Change window/width level by holding down mouse button and dragging
</p>
</li>
<li>
<p>
Ability to generate scout topogram from any angle of view and use it for navigation
</p>
</li>
<li>
<p>
Ability to measure HU over a circular region of interest
</p>
</li>
<li>
<p>
Support for simple geometric operations, such as measuring distances in physical units
</p>
</li>
<li>
<p>
Presets of CT window/width levels (abdomen, chest, bones, etc)
</p>
</li>
<li>
<p>
Drag and drop support with preview of the item&#8217;s content that is dragged
</p>
</li>
<li>
<p>
Floating layout when displaying a grid of images
</p>
</li>
<li>
<p>
Dockable windows (patient explorer widget, topogram)
</p>
</li>
<li>
<p>
Overlay of examination and patient details
</p>
</li>
</ul></div>
</div>
</div>
<div class="sect1">
<h2 id="_screenshots">3. Screenshots</h2>
<div class="sectionbody">
<div class="paragraph"><div class="title">Startup screen</div><p><span class="image">
<a class="image" href="images/1.png">
<img src="images/1.png" alt="Startup screen" width="320" />
</a>
</span></p></div>
<div class="paragraph"><div class="title">Patient explorer</div><p><span class="image">
<a class="image" href="images/2.png">
<img src="images/2.png" alt="Patient explorer" width="320" />
</a>
</span></p></div>
<div class="paragraph"><div class="title">Autocompletion</div><p><span class="image">
<a class="image" href="images/3.png">
<img src="images/3.png" alt="Autocompletion" width="320" />
</a>
</span></p></div>
<div class="paragraph"><div class="title">Floating grid layout</div><p><span class="image">
<a class="image" href="images/4.png">
<img src="images/4.png" alt="Floating grid layout" width="320" />
</a>
</span></p></div>
<div class="paragraph"><div class="title">Scout topogram</div><p><span class="image">
<a class="image" href="images/5.png">
<img src="images/5.png" alt="Scout topogram" width="320" />
</a>
</span></p></div>
<div class="paragraph"><div class="title">Measure distances</div><p><span class="image">
<a class="image" href="images/6.png">
<img src="images/6.png" alt="Measure distances" width="320" />
</a>
</span></p></div>
<div class="paragraph"><div class="title">Measure HU in a ROI</div><p><span class="image">
<a class="image" href="images/7.png">
<img src="images/7.png" alt="Measure HU in a ROI" width="320" />
</a>
</span></p></div>
</div>
</div>
<div class="sect1">
<h2 id="_current_state">4. Current state</h2>
<div class="sectionbody">
<div class="paragraph"><p>The project is no longer being actively developed, due to lack of time resources by its authors.</p></div>
</div>
</div>
</div>
<div id="footnotes"><hr /></div>
<div id="footer">
<div id="footer-text">
Last updated
 2017-01-06 00:00:07 EET
</div>
</div>
</body>
</html>
