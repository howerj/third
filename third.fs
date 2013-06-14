: immediate _read @ ! - * / <0 exit emit key _pick r> >r branch notbranch printnum

: r 1 exit

: true 1 exit
: false 0 exit

: _x 3 @ exit
: _y 4 @ exit
: _x! 3 ! exit
: _y! 4 ! exit
: swap _x! _y! _x _y exit
: dup _x! _x _x exit
: over _x! _y! _y _x _y exit
: + 0 swap - - exit
: inc dup @ 1 + swap ! exit
: h 0 exit
: , h @ ! h inc exit
: ' r @ @ dup 1 + r @ ! @ exit
: ; immediate ' exit , exit
: drop 0 * + exit
: tail r> r> drop >r exit
: minus 0 swap - exit
: bnot 1 swap - exit
: < - <0 exit
: > swap < exit
: <= 1 + < exit
: >= swap <= exit
: logical dup 0 < swap minus 0 < + exit
: not logical bnot exit
: = - not exit
: here h @ exit

: cpf 8 exit 
: state cpf ! exit
: ; immediate 
	' exit , 
	false state
exit

: [ immediate false state ;
: ] true state ;



: if immediate ' notbranch , here 0 , ;
: then immediate dup here swap - swap ! ;

: '\n' 10 ;
: '"' 34 ;
: 'space' 32 ;
: ')' 41 ;
: '0' 48 ;

: find-) key ')' = not if tail find-) then ;

: ( immediate find-) ;

( we should be able to do FORTH-style comments now )

( this works as follows: ( is an immediate word, so it gets
  control during compilation.  Then it simply reads in characters
  until it sees a close parenthesis.  once it does, it exits.
  if not, it pops off the return stack--manual tail recursion. )

( now that we've got comments, we can comment the rest of the code! )

: else immediate
  ' branch ,        ( compile a definite branch )
  here            ( push the backpatching address )
  0 ,            ( compile a dummy offset for branch )
  swap            ( bring old backpatch address to top )
  dup here swap -    ( calculate the offset from old address )
  swap !        ( put the address on top and store it )
;

: cr '\n' emit exit

: .
  dup 0 <
  if
    45 emit minus
  then
  printnum
  'space' emit
;


( the following routine takes a pointer to a string, and prints it,
  except for the trailing quote.  returns a pointer to the next word
  after the trailing quote )

: _print
  dup 1 +
  swap @
  dup '"' =
  if
    drop exit
  then
  emit
  tail _print
;

: print _print ;

  ( print the next thing from the instruction stream )
: immprint
  r @ @
  print
  r @ !
;

: find-"
  key dup ,
  '"' =
  if
    exit
  then
  tail find-"
;

: " immediate
  key drop
  ' immprint ,
  find-"
;

: _welcome " Welcome to THIRD.
Ok.
" ;

_welcome
