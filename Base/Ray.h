#pragma once
#include <memory>
#include "WJMath.h"

struct Ray
{
	Vector3 m_startPosition;
	Vector3 m_endPosition;
	Vector3 m_direction;
	bool m_bIsSegmentLine;

	Ray(Vector3 startPosition, Vector3 direction, Vector3 endPosition = {}, bool bIsSegmentLine = false) :
	m_startPosition(startPosition), m_direction(direction), m_endPosition (endPosition), m_bIsSegmentLine(bIsSegmentLine) {}
	Ray() : m_startPosition({ 0.0f, 0.0f, 0.0f }), m_direction({ 0.0f, 0.0f, 0.0f }), m_endPosition({ 0.0f, 0.0f, 0.0f }), m_bIsSegmentLine(false) {}
};