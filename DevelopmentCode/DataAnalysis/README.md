# some code to help looking at the data obtained by the logger

## Setup

- set up a google account for iridium data (if necessary, forward from main account: wheel -> settings -> *filters and blocked addresses* then *forwarding and pop / imap* )
- set up the google account for allowing POP messaging: wheel -> settings -> *forwarding and pop / imap* : enable POP
- set up the google account to accept 'unsafe' applications: wheel -> settings -> *accounts and imports* -> other google account settings -> apps with account access -> allow less secure apps: on
- create a local clone of the repository containing the data (example https://github.uio.no/jeanra/LoggerWaveIceSolarIridiumRaspberry )

## Use of the code

- check if some new data has been received: *python run_interaction_with_gmail.py* . This will download through POP the data received on the gmail account, add it to the local repo, and push. Always use data from the local repo, as data is only obtained once through POP.
- do some plots (NOTE: will need to adapt the parameters of the script, in particular date intervals for plotting): *python run_process_data.py* .
