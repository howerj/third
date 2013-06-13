: immediate _read @ ! - * / <0 exit emit key _pick r> >r branch notbranch printnum
: r 1 exit

: ] r @ 1 - r ! _read ]

: _main immediate r @ 7 ! ]
_main

: _x 3 @ exit
: _y 4 @ exit
: _x! 3 ! exit
: _y! 4 ! exit
: swap _x! _y! _x _y exit
: dup _x! _x _x exit
: over _x! _y! _y _x _y ;
: + 0 swap - - exit
: inc dup @ 1 + swap ! exit
: h 0 exit
: , h @ ! h inc exit
: ' r @ @ dup 1 + r @ ! @ exit
: ; immediate ' exit , exit
: drop 0 * + ;
: tail r> r> drop >r ;
: minus 0 swap - ;
: bnot 1 swap - ;
: < - <0 ;
: > swap < ;
: <= 1 + < ;
: >= swap <= ;
: logical dup 0 < swap minus 0 < + ;
: not logical bnot ;
: = - not ;
: here h @ ;

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

      
: execute
  8 !
  ' exit 9 !
  8 >r
;

: :: ;        ( :: is going to be a word that does ':' at runtime )

: fix-:: immediate 3 ' :: ! ;
fix-::

    ( Override old definition of ':' with a new one that invokes ] )
: : immediate :: ] ;

: command
  here 5 !        ( store dict pointer in temp variable )
  _read            ( compile a word )
            ( if we get control back: )
  here 5 @
  = if
    tail command    ( we didn't compile anything )
  then
  here 1 - h !        ( decrement the dictionary pointer )
  here 5 @        ( get the original value )
  = if
    here @        ( get the word that was compiled )
    execute        ( and run it )
  else
    here @        ( else it was an integer constant, so push it )
    here 1 - h !    ( and decrement the dictionary pointer again )
  then
  tail command
;


: [ immediate command ;
: _welcome " Welcome to THIRD.
Ok.
" ;

: ; immediate ' exit , command exit

[

_welcome
