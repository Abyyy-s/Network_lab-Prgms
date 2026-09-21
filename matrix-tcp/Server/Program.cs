
using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

class Program
{
    static bool IsUpper(int[,] matrix, int n)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < i; j++)
            {
                if (matrix[i, j] != 0)
                    return false;
            }
        }

        return true;
    }

    static bool IsLower(int[,] matrix, int n)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                if (matrix[i, j] != 0)
                    return false;
            }
        }

        return true;
    }

    static bool IsDiagonal(int[,] matrix, int n)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (i != j && matrix[i, j] != 0)
                    return false;
            }
        }

        return true;
    }

    static void Main()
    {
        TcpListener server = new TcpListener(
            IPAddress.Loopback,
            8080
        );

        server.Start();

        Console.WriteLine("Server waiting for connection...");

        using TcpClient client = server.AcceptTcpClient();

        Console.WriteLine("Client connected.");

        using NetworkStream stream = client.GetStream();

        // Receive N
        byte[] buffer = new byte[4];

        stream.ReadExactly(buffer, 0, 4);

        int n = BitConverter.ToInt32(buffer, 0);

        Console.WriteLine($"Received N = {n}");

        // Receive matrix
        int[,] matrix = new int[n, n];

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                stream.ReadExactly(buffer, 0, 4);

                matrix[i, j] = BitConverter.ToInt32(buffer, 0);
            }
        }

        // Display matrix
        Console.WriteLine("\nReceived Matrix:");

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                Console.Write($"{matrix[i, j],3} ");
            }

            Console.WriteLine();
        }

        // Identify matrix type
        string result;

        if (IsDiagonal(matrix, n))
            result = "Diagonal Matrix";
        else if (IsUpper(matrix, n))
            result = "Upper Triangular Matrix";
        else if (IsLower(matrix, n))
            result = "Lower Triangular Matrix";
        else
            result = "Not a special matrix";

        Console.WriteLine($"\nMatrix Type: {result}");

        // Send result
        byte[] resultBytes = Encoding.UTF8.GetBytes(result);

        stream.Write(resultBytes);

        server.Stop();
    }
}
