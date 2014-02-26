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

    # split string into array
    my $in_str = [split("", $expression)];
    my $el = "";
    CHAR: while (my $char = shift(@{$in_str})) {
        next CHAR if $char =~ /\s/;
        if ($operators->{$char} || $char eq "(" || $char eq ")") {
            if ($el) {
                push (@{$in}, $el);
            }

            push (@{$in}, $char);
            $el = "";
        } else {
            $el = $el . $char;
        }
    }
    push (@{$in}, $el);

    my $infix = [@{$in}];

    # convert array from infix to rpn
    SYMBOL: while (my $sym = shift(@{$in}) ) {
        if ($operators->{$sym}) {
            CHECK_OP_STACK: while (scalar(@{$op_stack})) {
                my $top_op = $op_stack->[scalar(@{$op_stack}) - 1];
    
                if ($operators->{$top_op} &&
                    (
                        $operators->{$sym}->{'precedence'} == $operators->{$top_op}->{'precedence'} &&
                        $operators->{$sym}->{'associativity'} eq 'left' ||
                        $operators->{$sym}->{'precedence'} < $operators->{$top_op}->{'precedence'}
                    )
                    ) {
                    
                    push (@{$out}, pop(@{$op_stack}));
                } else {
                    last CHECK_OP_STACK;
                }
            }

            push (@{$op_stack}, $sym);
        } elsif ($sym eq "(") {
            push (@{$op_stack}, $sym);
        } elsif ($sym eq ")") {
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
            push @{$out}, $sym;
        }
    }

    while (scalar(@{$op_stack})) {
        push (@{$out}, pop(@{$op_stack}));
    }

    if ($opts->{'debug'}) {
        print $expression . "\n";
        print join(" ", @{$infix}) . "\n";
        print join(" ", @{$out}) . "\n";
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

    while (my $el = shift(@{$rpn})) {
        if ($operators->{$el}) {
            my $tmp_value_1 = pop (@{$calc});
            my $tmp_value_2 = pop (@{$calc});

            if ($el eq "+") {
                push (@{$calc}, $tmp_value_2 + $tmp_value_1);
            } elsif ($el eq "-") {
                push (@{$calc}, $tmp_value_2 - $tmp_value_1);
            } elsif ($el eq "*") {
                push (@{$calc}, $tmp_value_2 * $tmp_value_1);
            } elsif ($el eq "/") {
                push (@{$calc}, $tmp_value_2 / $tmp_value_1);
            } elsif ($el eq "^") {
                push (@{$calc}, $tmp_value_2 ** $tmp_value_1);
            } else {
                Carp::confess("Programmer error: operator [$el] is not implemented.");
            }
        } else {
            push (@{$calc}, $el);
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
    $r = shunting_yard("2+3", {'debug' => 1});
    if (join(" ", @{$r->{'rpn'}}) eq "2 3 +") {
        print "test 1.1 passed\n";
    } else {
        Carp::confess("test 1.1 failed; expected rpn '2 3 +', got '" . Data::Dumper::Dumper($r));
    }
    $r1 = calc_rpn($r->{'rpn'}, {'debug' => 1});
    if ($r1 eq '5') {
        print "test 1.2 passed\n";
    } else {
        Carp::confess("test 1.2 failed; expected result '5', got '" . Data::Dumper::Dumper($r1));
    }
    printf("\n");

    $r = shunting_yard("2+3*5-12", {'debug' => 1});
    if (join(" ", @{$r->{'rpn'}}) eq "2 3 5 * + 12 -") {
        print "test 2.1 passed\n";
    } else {
        Carp::confess("test 2.1 failed; expected rpn '2 3 5 * + 12 -', got '" . Data::Dumper::Dumper($r));
    }
    $r1 = calc_rpn($r->{'rpn'}, {'debug' => 1});
    if ($r1 eq '5') {
        print "test 2.2 passed\n";
    } else {
        Carp::confess("test 2.2 failed; expected result '5', got '" . Data::Dumper::Dumper($r1));
    }
    printf("\n");

    $r = shunting_yard("3+4*2/(1-5)", {'debug' => 1});
    if (join(" ", @{$r->{'rpn'}}) eq "3 4 2 * 1 5 - / +") {
        print "test 3.1 passed\n";
    } else {
        Carp::confess("test 3.1 failed; expected rpn '3 4 2 * 1 5 - / +', got '" . Data::Dumper::Dumper($r));
    }
    $r1 = calc_rpn($r->{'rpn'}, {'debug' => 1});
    if ($r1 eq '1') {
        print "test 3.2 passed\n";
    } else {
        Carp::confess("test 3.2 failed; expected result '1', got '" . Data::Dumper::Dumper($r1));
    }
    printf("\n");

    $r = shunting_yard("3+4*2/(1-5)^2^3", {'debug' => 1});
    if (join(" ", @{$r->{'rpn'}}) eq "3 4 2 * 1 5 - 2 3 ^ ^ / +") {
        print "test 4.1 passed\n";
    } else {
        Carp::confess("test 4.1 failed; expected rpn '3 4 2 * 1 5 - 2 3 ^ ^ / +', got '" . Data::Dumper::Dumper($r));
    }
    $r1 = calc_rpn($r->{'rpn'}, {'debug' => 1});
    if ($r1 == (3+1/8192)) {
        print "test 4.2 passed\n";
    } else {
        Carp::confess("test 4.2 failed; expected result '" . (3+1/8192) . "', got '" . Data::Dumper::Dumper($r1));
    }
    printf("\n");
}
else {
    $r = shunting_yard($ARGV[0], {'debug' => 1});
    $r1 = calc_rpn($r->{'rpn'}, {'debug' => 1});
}
