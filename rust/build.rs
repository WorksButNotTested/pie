pub fn main () {
    println!("cargo:rustc-link-lib=static=c");
    println!("cargo:rustc-link-search=lib/");
}
