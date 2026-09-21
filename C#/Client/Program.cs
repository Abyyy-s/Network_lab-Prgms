
using System;
using System.Net.Sockets;
using System.Text;

class Program
{
    static void Main()
    {
        using TcpClient client = new TcpClient();

        client.Connect("127.0.0.1", 8080);

        Console.WriteLine("Connected to server.");

        using NetworkStream stream = client.GetStream();

        // Input N
        Console.Write("Enter the order of matrix: ");

        int n = int.Parse(Console.ReadLine()!);

        if (n <= 0 || n > 10)
        {
            Console.WriteLine("N must be between 1 and 10.");
            return;
        }

        // Send N
        byte[] nBytes = BitConverter.GetBytes(n);

        stream.Write(nBytes);

        // Generate matrix
        Random random = new Random();

        int[,] matrix = new int[n, n];

        Console.WriteLine("\nGenerated Matrix:");

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                matrix[i, j] = random.Next(1, 51);

                Console.Write($"{matrix[i, j],3} ");
            }

            Console.WriteLine();
        }

        // Send matrix
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                byte[] data = BitConverter.GetBytes(matrix[i, j]);

                stream.Write(data);
            }
        }

        Console.WriteLine("\nMatrix sent to server.");

        // Receive result
        byte[] buffer = new byte[100];

        int bytesRead = stream.Read(buffer, 0, buffer.Length);

        string result = Encoding.UTF8.GetString(
            buffer,
            0,
            bytesRead
        );

        Console.WriteLine(
            $"\nMatrix Type received from server: {result}"
        );
    }
}
