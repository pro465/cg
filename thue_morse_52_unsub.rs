// 0
struct O;
// 1
struct I;

struct Z;
struct S<T>(T);

trait Step<T> {
    type Output;
}

impl<T, U, V> Step<(T, U)> for S<V> 
    where
        S<V>: Step<T> + Step<U>,
    {
    type Output=(<S<V> as Step<T>>::Output, <S<V> as Step<U>>::Output);
}

impl<T: Step<(O, I)>> Step<O> for S<T> {
    type Output=<T as Step<(O, I)>>::Output;
}

impl<T: Step<(I, O)>> Step<I> for S<T> {
    type Output=<T as Step<(I, O)>>::Output;
}

impl<T> Step<T> for Z {
    type Output=T;
}

// run 11 steps 
fn main() {
    let _: <S<S<S<S<S<S<S<S<S<S<S<Z>>>>>>>>>>> as Step<O>>::Output = ();
}
