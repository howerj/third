: immediate _read @ ! - * / <0 exit emit key _pick r> >r branch notbranch
: r 1 exit

: ] r @ 1 - r ! _read ]

: _main immediate r @ 7 ! ]
_main

: _x 3 @ exit
: _y 4 @ exit
: _x! 3 ! exit
: _y! 4 ! exit


: swap _x! _y! _x _y exit

: + 0 swap - - exit

: dup _x! _x _x exit

: inc dup @ 1 + swap ! exit

: h 0 exit

: , h @ ! h inc exit

: ' r @ @ dup 1 + r @ ! @ exit

: ; immediate ' exit , exit

: drop 0 * + ;

: dec dup @ 1 - swap ! ;

: tail r> r> drop >r ;

: minus 0 swap - ;

: bnot 1 swap - ;

: < - <0 ;

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
  swap !        ( put the address on top and s>re it )
;

: over _x! _y! _y _x _y ;

: add
  _x!            ( save the pointer in a temp variable )
  _x @            ( get the value pointed to )
  +            ( add the incremement from on top of the stack )
  _x !            ( and save it )
;

: allot    h add ;


: mod _x! _y!        ( get x then y off of stack )
  _y
  _y _x / _x *        ( y - y / x * x )
  -
;

: cr '\n' emit exit

: printnum
  dup
  10 mod '0' +
  swap 10 / dup
  if
    printnum 0
  then
  drop emit
;

: .
  dup 0 <
  if
    45 emit minus
  then
  printnum
  'space' emit
;


: debugprint dup . cr ;

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

: do immediate
  ' swap ,        ( compile 'swap' to swap the limit and start )
  ' >r ,        ( compile to push the limit onto the return stack )
  ' >r ,        ( compile to push the start on the return stack )
  here            ( save this address so we can branch back to it )
;

: i r @ 1 - @ ;
: j r @ 3 - @ ;

: > swap < ;
: <= 1 + < ;
: >= swap <= ;

: inci 
  r @ 1 -     ( get the pointer to i )
  inc        ( add one to it )
  r @ 1 - @     ( find the value again )
  r @ 2 - @    ( find the limit value )
  <
  if
    r @ @ @ r @ @ + r @ ! exit        ( branch )
  then
  r> 1 +
  r> drop
  r> drop
  >r
;
      
: loop immediate ' inci , here - , ;

: loopexit

  r> drop        ( pop off our return address )
  r> drop        ( pop off i )
  r> drop        ( pop off the limit of i )
;            ( and return to the caller's caller routine )

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
  here 5 !        ( s>re dict pointer in temp variable )
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

: make-immediate    ( make a word just compiled immediate )
  here 1 -        ( back up a word in the dictionary )
  dup dup        ( save the pointer to here )
  h !            ( s>re as the current dictionary pointer )
  @            ( get the run-time code pointer )
  swap            ( get the dict pointer again )
  1 -            ( point to the compile-time code pointer )
  !            ( write run-time code pointer on compile-time pointer )
;

: <build immediate
  make-immediate    ( make the word compiled so far immediate )
  ' :: ,        ( compile '::', so we read next word )
  2 ,            ( compile 'pushint' )
  here 0 ,        ( write out a 0 but save address for does> )
  ' , ,            ( compile a push that address onto dictionary )
;

: does> immediate
  ' command ,        ( jump back into command mode at runtime )
  here swap !        ( backpatch the build> to point to here )
  2 ,            ( compile run-code primitive so we look like a word )
  ' r> ,        ( compile r>, which leaves var address on stack )
;

: var <build , does> ;
: constant <build , does> @ ;
: array <build allot does> + ;

: [ immediate command ;
: _welcome " Welcome to THIRD.
Ok.
" ;

: ; immediate ' exit , command exit

[

_welcome
