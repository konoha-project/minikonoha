import("Type.Complex");
import("JavaScript.Math");
import("Type.Float");
//import("konoha.new"); Don't use this
import("Syntax.JavaStyleClass");



void test() {
	Complex c = new Complex(1.57, 2.71);
	System.p(c.cabs());
	System.p(c.cabsf());
	System.p(c.cabsl());
}

test();
