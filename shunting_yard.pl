#!/usr/bin/perl

use strict;
use warnings;

use Data::Dumper;
use Carp;

my $operators = {
  '+' => { 'precedence' => 2, 'associativity' => 'left', },
  '-' => { 'precedence' => 2, 'associativity' => 'left', },
  '*' => { 'precedence' => 3, 'associativity' => 'left', },
  '/' => { 'precedence' => 3, 'associativity' => 'left', },
  '^' => { 'precedence' => 4, 'associativity' => 'right', },
  };

sub shunting_yard
{
    my ($expression, $opts) = @_;

    $opts = {} unless $opts;

    my $in = [];
    my $out = [];
    my $op_stack = [];

    # split string into array (tokenize)
    my $in_str = [split("", $expression)];
    my $token = "";

    my $previous_token = "";

    CHAR: while (my $char = shift(@{$in_str})) {
        next CHAR if $char =~ /\s/;
        
        if ($operators->{$previous_token} && ($char eq "+" || $char eq "-")) {
            # previos was [+-*/^], current is + or -
            $token = $token . $char;
            $previous_token = "";
        } elsif ($operators->{$char} || $char eq "(" || $char eq ")") {
            if ($token) {
                push (@{$in}, $token);
            }
            $token = "";

            push (@{$in}, $char);

            $previous_token = $char;
        } else {
            $token = $token . $char;
            $previous_token = "";
        }
    }
    if ($token) {
        push (@{$in}, $token);
    }

    my $infix = [@{$in}];

    # convert array from infix to rpn
    TOKEN: while (my $token = shift(@{$in}) ) {
        if ($operators->{$token}) {
            CHECK_OP_STACK: while (scalar(@{$op_stack})) {
                my $top_op = $op_stack->[scalar(@{$op_stack}) - 1];
    
                if ($operators->{$top_op} &&
                    (
                        $operators->{$token}->{'precedence'} == $operators->{$top_op}->{'precedence'} &&
                        $operators->{$token}->{'associativity'} eq 'left' ||
                        $operators->{$token}->{'precedence'} < $operators->{$top_op}->{'precedence'}
                    )
                    ) {
                    
                    push (@{$out}, pop(@{$op_stack}));
                } else {
                    last CHECK_OP_STACK;
                }
            }

            push (@{$op_stack}, $token);
        } elsif ($token eq "(") {
            push (@{$op_stack}, $token);
        } elsif ($token eq ")") {
            my $found_matching_paren;
            CHECK_OP_STACK: while (scalar(@{$op_stack})) {
                my $top_op = $op_stack->[scalar(@{$op_stack}) - 1];
                
                if ($top_op ne "(") {
                    push (@{$out}, pop(@{$op_stack}));
                } else {
                    $found_matching_paren = 1;
                    pop(@{$op_stack});
                    last CHECK_OP_STACK;
                }
            }
            Carp::confess("Invalid expression: can't find matching parenthesis")
                if !$found_matching_paren;
        } else {
            push @{$out}, $token;
        }
    }

    while (scalar(@{$op_stack})) {
        push (@{$out}, pop(@{$op_stack}));
    }

    if ($opts->{'debug'}) {
        print "ORIGINAL: " . $expression . "\n";
        print "TOKENIZED: " . join(" ", @{$infix}) . "\n";
        print "RPN: " . join(" ", @{$out}) . "\n";
    }

    return {
        'original' => $expression,
        'infix' => $infix,
        'rpn' => $out,
        };
}

sub calc_rpn
{
    my ($rpn, $opts) = @_;

    $opts = {} unless $opts;

    my $calc = [];

    while (my $token = shift(@{$rpn})) {
        if ($operators->{$token}) {
            my $tmp_value_1 = pop (@{$calc});
            my $tmp_value_2 = pop (@{$calc});

            if ($token eq "+") {
                push (@{$calc}, $tmp_value_2 + $tmp_value_1);
            } elsif ($token eq "-") {
                push (@{$calc}, $tmp_value_2 - $tmp_value_1);
            } elsif ($token eq "*") {
                push (@{$calc}, $tmp_value_2 * $tmp_value_1);
            } elsif ($token eq "/") {
                push (@{$calc}, $tmp_value_2 / $tmp_value_1);
            } elsif ($token eq "^") {
                push (@{$calc}, $tmp_value_2 ** $tmp_value_1);
            } else {
                Carp::confess("Programmer error: operator [$token] is not implemented.");
            }
        } else {
            push (@{$calc}, $token);
        }
    }

    if ($opts->{'debug'}) {
        print $calc->[0] . "\n";
    }

    return $calc->[0];
}

my $r;
my $r1;

if (!$ARGV[0]) {
    printf("usage: shuntint_yard.pl <test|EXPRESION>\n");
    exit(0);
}

if ($ARGV[0] eq 'test') {
    my $tests = [
        { 'expression' => "2+3",             'expected_rpn' => "2 3 +",                     'expected_result' => 5, },
        { 'expression' => "2+3*5-12",        'expected_rpn' => "2 3 5 * + 12 -",            'expected_result' => 5, },
        { 'expression' => "3+4*2/(1-5)",     'expected_rpn' => "3 4 2 * 1 5 - / +",         'expected_result' => 1, },
        { 'expression' => "3+4*2/(1-5)^2^3", 'expected_rpn' => "3 4 2 * 1 5 - 2 3 ^ ^ / +", 'expected_result' => (3+1/8192), },
        { 'expression' => "1+-1",            'expected_rpn' => "1 -1 +",                    'expected_result' => 0, },
        { 'expression' => "3+2*5-2/2",       'expected_rpn' => "3 2 5 * + 2 2 / -",         'expected_result' => 12, },
        { 'expression' => "2^3+3^2",         'expected_rpn' => "2 3 ^ 3 2 ^ +",             'expected_result' => 17, },
        { 'expression' => "2^(3+1)-(1+1)*-1",'expected_rpn' => "2 3 1 + ^ 1 1 + -1 * -",    'expected_result' => 18, },
        { 'expression' => "(((1)))",         'expected_rpn' => "1",                         'expected_result' => 1, },
        ];

    my $i = 0;
    foreach my $test (@{$tests}) {
        $i++;

        $r = shunting_yard($test->{'expression'}, {'debug' => 1});
        if (join(" ", @{$r->{'rpn'}}) eq $test->{'expected_rpn'}) {
            print "test $i.1 passed\n";
        } else {
            Carp::confess("test $i.1 failed; expected rpn [" . $test->{'expected_rpn'} . "], got: " . Data::Dumper::Dumper($r));
        }
    
        $r1 = calc_rpn($r->{'rpn'}, {'debug' => 1});
        if ($r1 == $test->{'expected_result'}) {
            print "test $i.2 passed\n";
        } else {
            Carp::confess("test $i.2 failed; expected result [" . $test->{'expected_result'} . "], got " . Data::Dumper::Dumper($r1));
        }
        printf("\n");
    }
}
else {
    $r = shunting_yard($ARGV[0], {'debug' => 1});
    $r1 = calc_rpn($r->{'rpn'}, {'debug' => 1});
}
