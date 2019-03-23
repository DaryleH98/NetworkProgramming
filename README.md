# NetworkProgramming
Web Remote Command Server that builds a simple HTTP server where it's job is to get a command from the URL, run it on the server, and report the results back to user in the browser. By results we mean the information that is normally printed to stdout and stderr as the result of running the command. This application is useful for example if you need to retrieve information while you're away from your target machine using only a web browser. We also whitelist connections to the server in order to decrease security risk to our server machine.
