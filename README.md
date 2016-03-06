# ENIGMA MACHINE BOMBE AND SOLVER

This software is in heavy development.
It can be used as a enigma encrypt machine or as an enigma bombe.

The bombe is based on [known-plaintext attack (KPA)](http://en.wikipedia.org/wiki/Known-plaintext_attack) for this we need a crib text.
This attack is assisted for stadistical and heuristics to help the KPA.


## USAGE
Run enigma to get a hacky CLI :-)

## EXAMPLE

	   _____  _____________  ______     ________  __ _   _________ 
	  / __/ |/ /  _/ ___/  |/  / _ |   / __/ __ \/ /| | / / __/ _ \
	 / _//    // // (_ / /|_/ / __ |  _\ \/ /_/ / /_| |/ / _// , _/
	/___/_/|_/___/\___/_/  /_/_/ |_| /___/\____/____/___/___/_/|_| 
	-----------------------------------------------------------------
	
	e)ncrypt
	d)ecrypt
	s)et properties
	
	ENIGMA> d
	Encrypted Text: KOXERTFYVVLLHBOJNGJVEUWVYBGGCPYKWXXIFVMTFSGXHTQSAY
	Crib Text:      WIRWER
	
	[I]  00% Completed - Checking wheels 123
	[R] Wheel: 123 Start: AAA Rotor: AAA Stecker: ""	Decoded String: WIRWERDENMORGENUMUHRNACHMITTAGSDIETRUPPENANGREIFEN Rank: 9
	[I] Has been found a coincidence, bruteforcing enigma plug.
	[I] This process can take a very long time.
	[I] Bruteforce plug: "AB"
	[R] Wheel: 123 Start: AAA Rotor: AAA Stecker: "AB"	Decoded String: WIRWERDENMORGTNUMUHRNBCHMXTTBGSDIETRUPPENBNGREIFQN Rank: 6
	[I] Bruteforce plug: "ABCD"
	[R] Wheel: 123 Start: AAA Rotor: AAA Stecker: "ABCD"	Decoded String: WIRWERCENMORGTNUMUHRNBDHMXTTNGSCIETRUPPENBNGREIFQN Rank: 4
