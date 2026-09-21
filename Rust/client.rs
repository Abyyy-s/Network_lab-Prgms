use std::io::{Read, Write};
use std::net::TcpStream;
use std::time::{SystemTime, UNIX_EPOCH};

fn main() {
    // Connect to server
    let mut stream = TcpStream::connect("127.0.0.1:8080")
    .expect("Failed to connect to server");

    println!("Connected to server.");

    // Input N
    let mut input = String::new();

    println!("Enter the order of matrix:");
    std::io::stdin()
    .read_line(&mut input)
    .expect("Failed to read input");

    let n: usize = input
    .trim()
    .parse()
    .expect("Please enter a valid number");

    if n == 0 {
        println!("N must be greater than 0.");
        return;
    }

    if n > 10 {
        println!("N must be <= 10.");
        return;
    }

    // Send N
    let n_bytes = (n as u32).to_ne_bytes();

    stream
    .write_all(&n_bytes)
    .expect("Failed to send N");

    // Simple pseudo-random generator
    let mut seed = SystemTime::now()
    .duration_since(UNIX_EPOCH)
    .unwrap()
    .subsec_nanos();

    let mut matrix = vec![vec![0i32; n]; n];

    println!("\nGenerated Matrix:");

    for i in 0..n {
        for j in 0..n {
            // Generate number from 1 to 50
            seed = seed
            .wrapping_mul(1664525)
            .wrapping_add(1013904223);

            matrix[i][j] = (seed % 50 + 1) as i32;

            print!("{:3} ", matrix[i][j]);
        }

        println!();
    }

    // Send matrix
    for i in 0..n {
        for j in 0..n {
            stream
            .write_all(&matrix[i][j].to_ne_bytes())
            .expect("Failed to send matrix");
        }
    }

    println!("\nMatrix sent to server.");

    // Receive result
    let mut buffer = [0u8; 100];

    let size = stream
    .read(&mut buffer)
    .expect("Failed to receive result");

    let result = String::from_utf8_lossy(&buffer[..size]);

    println!(
        "\nMatrix Type received from server: {}",
        result
    );
}
