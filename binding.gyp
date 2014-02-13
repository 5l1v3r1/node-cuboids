{
  "targets": [
    {
      "target_name": "cuboids",
      "sources": [
        "<!@(ls -1 ./Cuboids/representation/*.c)",
        "<!@(ls -1 ./sources/*.cpp)"
      ],
      "include_dirs": ["./Cuboids", "./sources"]
    }
  ]
}