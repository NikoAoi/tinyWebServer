#!/usr/bin/perl -Tw

use strict;
use CGI;

my($cgi) = new CGI;

print $cgi->header;
my($name) = "niko";
$name = $cgi->param('username') if defined $cgi->param('username');

print $cgi->start_html(-title => uc($name));
print $cgi->h1("Hello! Your name is $name");
print $cgi->end_html;
