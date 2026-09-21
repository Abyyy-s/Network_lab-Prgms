use std::io::{Read, Write};
use std::net::TcpListener;

fn is_upper(matrix: &Vec<Vec<i32>>, n: usize) -> bool {
    for i in 0..n {
        for j in 0..i {
            if matrix[i][j] != 0 {
                return false;
            }
        }
    }
    true
}

fn is_lower(matrix: &Vec<Vec<i32>>, n: usize) -> bool {
    for i in 0..n {
        for j in (i + 1)..n {
            if matrix[i][j] != 0 {
                return false;
            }
        }
    }
    true
}

fn is_diagonal(matrix: &Vec<Vec<i32>>, n: usize) -> bool {
    for i in 0..n {
        for j in 0..n {
            if i != j && matrix[i][j] != 0 {
                return false;
            }
        }
    }
    true
}

fn main() {
    let listener = TcpListener::bind("127.0.0.1:8080")
    .expect("Failed to bind");

    println!("Server waiting for connection...");

    let (mut stream, _) = listener
    .accept()
    .expect("Failed to accept connection");

    println!("Client connected.");

    // Receive N
    let mut n_buffer = [0u8; 4];
    stream.read_exact(&mut n_buffer)
    .expect("Failed to receive N");

    let n = u32::from_ne_bytes(n_buffer) as usize;

    println!("Received N = {}", n);

    // Receive matrix
    let mut matrix = vec![vec![0i32; n]; n];

    for i in 0..n {
        for j in 0..n {
            let mut buffer = [0u8; 4];

            stream.read_exact(&mut buffer)
            .expect("Failed to receive matrix");

            matrix[i][j] = i32::from_ne_bytes(buffer);
        }
    }

    // Display matrix
    println!("\nReceived Matrix:");

    for row in &matrix {
        for value in row {
            print!("{:3} ", value);
        }
        println!();
    }

    // Identify matrix type
    let result;

    if is_diagonal(&matrix, n) {
        result = "Diagonal Matrix";
    } else if is_upper(&matrix, n) {
        result = "Upper Triangular Matrix";
    } else if is_lower(&matrix, n) {
        result = "Lower Triangular Matrix";
    } else {
        result = "Not a special matrix";
    }

    println!("\nMatrix Type: {}", result);

    // Send result
    stream
    .write_all(result.as_bytes())
    .expect("Failed to send result");
}
