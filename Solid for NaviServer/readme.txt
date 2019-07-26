REQUIRED STEPS TO REPRODUCE THE CONFIGURATION

1. Install dependencies - the file dependencies.txt contains Ubuntu commands to install the different software components
2. Configure Solid - again in dependencies.txt the configuration can be found
3. Replace the OpenACS configuration file with the config-oacs-5-9-1.tcl from the archive
4. In order to fix the authentication issue: 
	4.1 Open the Developer Tools of the browser and find the cookie header while in /private/ page.
	4.2 Find in revproxy-procs.tcl the function 'upstream'
	4.3 After the "set queryHeaders [ns_conn headers]" line append with:
		set cookie [ns_set iget $queryHeaders Cookie]
		append cookie "; connect.sid=PASTE_COOKIE_HERE"
		ns_set update $queryHeaders Cookie $cookie