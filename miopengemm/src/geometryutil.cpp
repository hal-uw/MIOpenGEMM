/*******************************************************************************
 * Copyright (C) 2017 Advanced Micro Devices, Inc. All rights reserved.
 *******************************************************************************/
#include <tuple>
#include <miopengemm/geometryutil.hpp>

namespace MIOpenGEMM
{

std::vector<Geometry>
get_from_m_n_k_tA_tB(const std::vector<std::tuple<size_t, size_t, size_t, bool, bool>>& basicgeos,
                     size_t wSpaceSize)
{

  std::vector<Geometry> geometries;
  bool                  isColMajor = true;
  bool                  tA;
  bool                  tB;
  bool                  tC = false;
  size_t                lda;
  size_t                ldb;
  size_t                ldc;
  size_t                m;
  size_t                n;
  size_t                k;

  bool ldx_offset = false;

  for (auto& problem : basicgeos)
  {
    std::tie(m, n, k, tA, tB) = problem;
    lda = (tA == isColMajor ? k : m) + (ldx_offset == 1 ? 5 : 0);
    ldb = (tB == isColMajor ? n : k) + (ldx_offset == 1 ? 7 : 0);
    ldc = (tC == isColMajor ? n : m) + (ldx_offset == 1 ? 13 : 0);
    geometries.emplace_back(isColMajor, tA, tB, tC, lda, ldb, ldc, m, n, k, wSpaceSize, 'f');
  }
  return geometries;
}

std::vector<Geometry> get_from_m_n_k_ldaABC_tA_tB(
  const std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, bool, bool>>&
         basicgeos,
  size_t wSpaceSize)
{

  std::vector<Geometry> geometries;
  bool                  isColMajor = true;
  bool                  tA;
  bool                  tB;
  bool                  tC = false;
  size_t                lda;
  size_t                ldb;
  size_t                ldc;
  size_t                m;
  size_t                n;
  size_t                k;

  for (auto& problem : basicgeos)
  {
    std::tie(m, n, k, lda, ldb, ldc, tA, tB) = problem;
    geometries.emplace_back(isColMajor, tA, tB, tC, lda, ldb, ldc, m, n, k, wSpaceSize, 'f');
  }
  return geometries;
}
}
